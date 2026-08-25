> File uploads are one of the most dangerous features to implement. A carelessly implemented upload endpoint is a direct path to remote code execution. This is especially relevant for Paideon's Vault feature.

---

## Table of Contents

1. [Why File Uploads Are Dangerous](#1-why-file-uploads-are-dangerous)
2. [File Type Validation](#2-file-type-validation)
3. [Magic Bytes — Real File Type Detection](#3-magic-bytes--real-file-type-detection)
4. [Filename Sanitization](#4-filename-sanitization)
5. [Size Limits](#5-size-limits)
6. [Storage Location](#6-storage-location)
7. [Virus/Malware Scanning](#7-virusmalware-scanning)
8. [Serving Uploaded Files Safely](#8-serving-uploaded-files-safely)
9. [Image-Specific Security](#9-image-specific-security)
10. [Complete Secure Upload Pipeline](#10-complete-secure-upload-pipeline)

---

## 1. Why File Uploads Are Dangerous

```
Without proper controls, an attacker can upload:
  → A PHP/Node.js script disguised as an image
    If served from your domain and executed by the server → remote code execution

  → A malicious PDF with JavaScript embedded
    Victims download it → JavaScript executes in their PDF reader

  → An SVG file containing XSS
    SVG is XML and can contain <script> tags
    If served with Content-Type: image/svg+xml → executes in browser

  → A ZIP bomb (1KB zip that expands to 1TB)
    Unzipping it crashes the server

  → A file with path traversal in the filename
    "../../../../etc/passwd" → server writes outside intended directory

  → An executable file (.exe, .sh)
    Downloaded by victim → runs on their machine
```

---

## 2. File Type Validation

Never trust the `Content-Type` header from the client — it can be set to anything.

```ts
// WRONG — trusting client-provided content type
@Post("upload")
async upload(@UploadedFile() file: Express.Multer.File) {
  if (file.mimetype !== "application/pdf") {
    throw new BadRequestException("Only PDFs allowed");
  }
  // file.mimetype is whatever the client sent — attacker sent "application/pdf"
  // but the actual file is a PHP script
}
```

```ts
// Allowlist of permitted types and their extensions
const ALLOWED_TYPES: Record<string, string[]> = {
  "image/jpeg": [".jpg", ".jpeg"],
  "image/png": [".png"],
  "image/webp": [".webp"],
  "application/pdf": [".pdf"],
  "video/mp4": [".mp4"],
  // NOT allowed: .php, .js, .html, .svg, .exe, .sh, .py, .rb, etc.
};

function isAllowedExtension(filename: string): boolean {
  const ext = path.extname(filename).toLowerCase();
  return Object.values(ALLOWED_TYPES).flat().includes(ext);
}
```

---

## 3. Magic Bytes — Real File Type Detection

Every file format has a signature in the first few bytes — "magic bytes". JPEG always starts with `FF D8 FF`. PNG starts with `89 50 4E 47`. Use this to detect the true type regardless of what the client claims.

```bash
npm install file-type
```

```ts
import { fileTypeFromBuffer } from "file-type";

async function validateFileType(buffer: Buffer, allowedMimeTypes: string[]): Promise<string> {
  const detected = await fileTypeFromBuffer(buffer);

  if (!detected) {
    throw new BadRequestException("Cannot determine file type");
  }

  if (!allowedMimeTypes.includes(detected.mime)) {
    throw new BadRequestException(
      `File type ${detected.mime} is not allowed`
    );
  }

  return detected.mime;
}

// Usage in controller
@Post("upload")
async upload(@UploadedFile() file: Express.Multer.File) {
  const detectedType = await validateFileType(
    file.buffer,
    ["image/jpeg", "image/png", "application/pdf"]
  );
  // detectedType is trusted — based on file content, not client claim
}
```

---

## 4. Filename Sanitization

User-provided filenames can contain path traversal sequences, null bytes, or special characters.

```ts
import path from "path";
import { v4 as uuidv4 } from "uuid";

function sanitizeFilename(originalName: string): string {
  // Extract only the base name — strip any directory components
  const baseName = path.basename(originalName);

  // Remove characters that could cause issues
  const safeName = baseName
    .replace(/[^a-zA-Z0-9.\-_]/g, "_") // allow only alphanumeric, ., -, _
    .replace(/\.{2,}/g, ".") // collapse multiple dots (../../)
    .slice(0, 100); // limit length

  // Better: generate a completely new name — don't trust the original at all
  const ext = path.extname(originalName).toLowerCase();
  const newName = `${uuidv4()}${ext}`;
  return newName;
}
// "../../etc/passwd"  → "a3b4c5d6-...-uuid.passwd" stored safely
// "photo.jpg"         → "a3b4c5d6-...-uuid.jpg" stored safely
// "malware.php.jpg"   → "a3b4c5d6-...-uuid.jpg" (extension validated separately)
```

**Best practice:** Generate a completely new UUID-based filename. Store the original name as metadata in the database for display, but never use it for storage.

---

## 5. Size Limits

```ts
// NestJS Multer configuration
import { MulterModule } from "@nestjs/platform-express";

@Module({
  imports: [
    MulterModule.register({
      limits: {
        fileSize: 10 * 1024 * 1024,  // 10MB per file
        files: 5,                     // max 5 files per request
        fieldSize: 1 * 1024 * 1024,  // 1MB for non-file fields
      },
    }),
  ],
})

// Per-route limits
@Post("upload-avatar")
@UseInterceptors(
  FileInterceptor("file", {
    limits: { fileSize: 2 * 1024 * 1024 },  // 2MB for profile images
  })
)
```

Also set at the Nginx level — this prevents large uploads from even reaching your application:

```nginx
client_max_body_size 10m;  # Maximum request body size
```

---

## 6. Storage Location

**Never store uploads in a web-accessible directory.**

```
BAD:
  Upload stored at: /var/www/html/uploads/photo.php
  Accessible at:    https://example.com/uploads/photo.php
  Nginx serves it:  If configured as PHP FastCGI, executes the script!
  Result:           Remote code execution

GOOD:
  Upload stored at: /var/uploads/photo.jpg  (outside web root)
  or: Cloudflare R2 / AWS S3 (object storage — not a filesystem)

  Files are served through your application (controlled endpoint)
  or via signed URLs from object storage
  Your application controls access checks before serving
```

### Using Cloudflare R2 / AWS S3

```ts
import {
  S3Client,
  PutObjectCommand,
  GetObjectCommand,
} from "@aws-sdk/client-s3";
import { getSignedUrl } from "@aws-sdk/s3-request-presigner";

const s3 = new S3Client({
  endpoint: process.env.R2_ENDPOINT,
  credentials: {
    accessKeyId: process.env.R2_ACCESS_KEY_ID,
    secretAccessKey: process.env.R2_SECRET_ACCESS_KEY,
  },
  region: "auto",
});

// Upload
async function uploadFile(
  key: string, // e.g., "schools/school_A/vault/uuid.pdf"
  buffer: Buffer,
  contentType: string,
) {
  await s3.send(
    new PutObjectCommand({
      Bucket: process.env.R2_BUCKET,
      Key: key,
      Body: buffer,
      ContentType: contentType,
      // Never set ACL: "public-read" for private files
      ServerSideEncryption: "AES256",
    }),
  );
}

// Generate signed URL (time-limited access)
async function getSignedDownloadUrl(key: string): Promise<string> {
  const command = new GetObjectCommand({
    Bucket: process.env.R2_BUCKET,
    Key: key,
  });
  return getSignedUrl(s3, command, { expiresIn: 3600 }); // 1 hour
}
```

The signed URL approach means:

- Files are never publicly accessible
- Access requires calling your API (where you enforce authorization)
- API generates a time-limited signed URL
- User downloads directly from R2 (not through your server — no bandwidth cost)

---

## 7. Virus/Malware Scanning

For a production system handling user uploads, integrate antivirus scanning.

```bash
# ClamAV — open source, free
apt install clamav clamav-daemon
freshclam  # update virus definitions

# Scan a file
clamscan /path/to/uploaded/file
# Returns: OK or FOUND (malware detected)
```

```ts
import NodeClam from "clamscan";

const clamscan = await new NodeClam().init({
  clamdscan: { host: "localhost", port: 3310 },
});

async function scanFile(filePath: string): Promise<void> {
  const { isInfected, viruses } = await clamscan.isInfected(filePath);
  if (isInfected) {
    await fs.unlink(filePath); // delete immediately
    throw new BadRequestException(`Malware detected: ${viruses.join(", ")}`);
  }
}
```

For cloud storage: Cloudflare and AWS S3 have optional malware scanning integrations.

---

## 8. Serving Uploaded Files Safely

If you serve files through your application (not via signed URLs):

```ts
@Get("files/:fileId")
@UseGuards(JwtAuthGuard)
async serveFile(
  @Param("fileId") fileId: string,
  @CurrentUser() user: JwtPayload,
  @Res() res: Response
) {
  // 1. Verify user has access to this file
  const file = await this.fileService.findWithAuthCheck(fileId, user);

  // 2. Read the file content
  const buffer = await this.storageService.getFile(file.storagePath);

  // 3. Set safe headers
  res.setHeader("Content-Type", file.mimeType);
  res.setHeader("Content-Length", buffer.length);

  // Force download — don't allow browser to render/execute
  res.setHeader("Content-Disposition", `attachment; filename="${encodeURIComponent(file.originalName)}"`);

  // Prevent the file from running scripts even if opened in browser
  res.setHeader("Content-Security-Policy", "default-src 'none'");
  res.setHeader("X-Content-Type-Options", "nosniff");

  res.send(buffer);
}
```

Key: `Content-Disposition: attachment` forces download instead of browser rendering. A PHP script served as attachment won't execute — the browser downloads it as a file.

---

## 9. Image-Specific Security

Images can contain embedded content, EXIF data, or be specially crafted to exploit image parsers.

```ts
import sharp from "sharp";

async function processImage(buffer: Buffer): Promise<Buffer> {
  // Re-encode the image through sharp
  // This:
  //   → Strips EXIF data (location, camera info, personal data)
  //   → Removes any embedded scripts or malicious payloads
  //   → Validates the image is actually parseable
  //   → Normalizes to a known-safe format

  return sharp(buffer)
    .resize(2048, 2048, { fit: "inside", withoutEnlargement: true })
    .jpeg({ quality: 85 }) // re-encode as JPEG — strips embedded content
    .toBuffer();
}
```

Re-encoding through a trusted image library neutralizes most image-based attacks because the library only preserves valid image data — not embedded scripts or malicious EXIF.

---

## 10. Complete Secure Upload Pipeline

```ts
@Post("vault/upload")
@UseGuards(JwtAuthGuard, RolesGuard)
@Roles(UserRole.LIBRARIAN, UserRole.ADMIN)
@UseInterceptors(FileInterceptor("file", { limits: { fileSize: 50 * 1024 * 1024 } }))
async uploadVaultFile(
  @UploadedFile() file: Express.Multer.File,
  @CurrentUser() user: JwtPayload,
  @Body() dto: UploadFileDto,
) {
  // 1. Validate file exists
  if (!file) throw new BadRequestException("No file provided");

  // 2. Validate file type via magic bytes
  const detectedType = await validateFileType(file.buffer, [
    "application/pdf",
    "image/jpeg",
    "image/png",
    "application/vnd.openxmlformats-officedocument.wordprocessingml.document",
  ]);

  // 3. Validate extension matches detected type
  const ext = path.extname(file.originalname).toLowerCase();
  if (!ALLOWED_TYPES[detectedType]?.includes(ext)) {
    throw new BadRequestException("File extension does not match content");
  }

  // 4. Scan for malware
  await scanFile(file.buffer);

  // 5. Process images (strip EXIF, re-encode)
  let processedBuffer = file.buffer;
  if (detectedType.startsWith("image/")) {
    processedBuffer = await processImage(file.buffer);
  }

  // 6. Generate safe storage path
  const storedName = `${uuidv4()}${ext}`;
  const storagePath = `schools/${user.schoolId}/vault/${storedName}`;

  // 7. Upload to object storage
  await this.storageService.upload(storagePath, processedBuffer, detectedType);

  // 8. Save record
  return this.db.vaultFile.create({
    data: {
      schoolId: user.schoolId,
      uploadedById: user.sub,
      originalName: path.basename(file.originalname), // stored for display
      storagePath,
      mimeType: detectedType,
      sizeBytes: processedBuffer.length,
      title: dto.title,
    },
  });
}
```

---

## Summary

```
File upload risks:
  RCE via uploaded scripts, XSS via SVG, path traversal in filenames,
  zip bombs, malware distribution

Validation pipeline:
  1. File type via magic bytes (not client Content-Type)
  2. Extension matches detected type
  3. Malware scan (ClamAV)
  4. Re-encode images (strips embedded content, EXIF)
  5. Sanitize/replace filename (UUID-based)
  6. Size limits (Multer + Nginx)
  7. Store outside web root (R2/S3, not /var/www)

Serving files:
  Verify authorization before serving
  Content-Disposition: attachment (force download, no browser execution)
  Content-Security-Policy: default-src 'none'
  Signed URLs for direct-from-storage downloads
```

---
