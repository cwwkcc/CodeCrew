> Mobile apps extend your attack surface into devices you don't control. The API is the same backend, but the client is different — no browser security model, no same-origin policy, different storage, and the binary is in an attacker's hands. This covers secure token storage, certificate pinning, and mobile-specific threats.

---

## Table of Contents

1. [Mobile vs Web Security Model](#1-mobile-vs-web-security-model)
2. [Secure Token Storage on Mobile](#2-secure-token-storage-on-mobile)
3. [Certificate Pinning on Mobile](#3-certificate-pinning-on-mobile)
4. [Biometric Authentication on Mobile](#4-biometric-authentication-on-mobile)
5. [Jailbreak and Root Detection](#5-jailbreak-and-root-detection)
6. [Binary Reverse Engineering](#6-binary-reverse-engineering)
7. [Mobile-Specific API Security](#7-mobile-specific-api-security)
8. [Deep Links and URL Scheme Security](#8-deep-links-and-url-scheme-security)
9. [Data in Transit and at Rest](#9-data-in-transit-and-at-rest)

---

## 1. Mobile vs Web Security Model

```
Web app (browser):
  Code runs in a sandboxed environment
  Same-origin policy prevents cross-origin data access
  DOM, cookies, localStorage are within the browser sandbox
  Attacker needs to inject code to steal data (XSS)

Mobile app:
  Code runs in an OS-level sandbox (per-app)
  No browser same-origin policy
  Files, keychain/keystore are accessible to the app
  On jailbroken/rooted device: sandbox is weakened
  The APK/IPA binary can be extracted, decompiled, analyzed
  Network traffic can be intercepted with proxy tools (Burp, mitmproxy)
```

The fundamental difference: **the attacker can have your app binary**. They can run it in an emulator, hook into it with Frida, or proxy all network traffic. Your backend must assume the client is potentially compromised.

---

## 2. Secure Token Storage on Mobile

### What's Available

**iOS — Keychain:**

```
Secure Enclave-backed storage (on devices with A7+ chips)
Encrypted by the device's hardware root key
Persists across app reinstalls (unless explicitly deleted)
Can be biometric-protected: access requires Touch ID/Face ID
Different keychain groups = different apps can't read each other's data

Accessibility levels:
  kSecAttrAccessibleWhenUnlockedThisDeviceOnly  ← recommended: unlocked + this device
  kSecAttrAccessibleAfterFirstUnlock            ← for background tasks
  kSecAttrAccessibleAlways                      ← weakest, avoid
```

**Android — Keystore + EncryptedSharedPreferences:**

```
Android Keystore: hardware-backed key storage
  Keys stored in secure hardware (TEE or StrongBox)
  Keys cannot be extracted from the device
  
EncryptedSharedPreferences: encrypted key-value store
  Encryption keys stored in Android Keystore
  Data encrypted at rest in the app's storage directory

Avoid:
  Regular SharedPreferences (unencrypted)
  Internal file storage without encryption
  External storage (SD card — accessible to other apps)
```

### React Native — Secure Storage

```ts
// react-native-keychain — wraps iOS Keychain + Android Keystore
import * as Keychain from "react-native-keychain";

// Store access token
await Keychain.setGenericPassword(
  "accessToken",   // username/key
  token,           // value (the actual token)
  {
    accessible: Keychain.ACCESSIBLE.WHEN_UNLOCKED_THIS_DEVICE_ONLY,
    // iOS: biometrics not required just to read (access-on-open)
  }
);

// Retrieve
const credentials = await Keychain.getGenericPassword();
if (credentials) {
  const token = credentials.password;
}

// Delete on logout
await Keychain.resetGenericPassword();

// For biometric-protected storage (requires biometric to read):
await Keychain.setGenericPassword("refreshToken", refreshToken, {
  accessControl: Keychain.ACCESS_CONTROL.BIOMETRY_CURRENT_SET,
  accessible: Keychain.ACCESSIBLE.WHEN_UNLOCKED_THIS_DEVICE_ONLY,
});
```

### What Not to Use

```
AsyncStorage (React Native):
  Unencrypted
  Stored as plain text files in the app's data directory
  On rooted/jailbroken devices: readable without authentication
  NEVER store tokens here

Redux store / in-memory state:
  Fine for access tokens during an active session
  Lost when app is backgrounded and killed (acceptable — will refresh)
  Not for refresh tokens (user would need to re-login every app close)

localStorage / sessionStorage (React Native WebView):
  Same as web — accessible to JavaScript
  If the WebView loads untrusted content → XSS can read it
```

---

## 3. Certificate Pinning on Mobile

Certificate pinning ensures the app only trusts your specific certificate or public key, ignoring any other CA-signed certificate for your domain.

```
Without pinning:
  Tester installs a proxy CA on their device
  Burp Suite / mitmproxy issues a certificate for api.yourschool.lk
  Device trusts it (because it trusts the installed CA)
  Tester can intercept and read all HTTPS traffic

With pinning:
  App hardcodes: "api.yourschool.lk's public key must be abc123"
  Proxy cert has a different public key
  App rejects the connection → pinning error
  Tester cannot intercept (without advanced techniques like Frida)
```

### React Native — SSL Pinning

```ts
// react-native-ssl-public-key-pinning
import { fetch } from "react-native-ssl-public-key-pinning";

const pins = {
  "api.yourschool.lk": {
    includeSubdomains: false,
    publicKeyHashes: [
      "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA=", // current cert's public key hash
      "BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB=", // backup cert (for rotation)
    ],
  },
};

// Use this fetch everywhere instead of native fetch
const response = await fetch("https://api.yourschool.lk/api/data", {
  method: "GET",
  headers: { Authorization: `Bearer ${token}` },
  // pins are checked automatically
}, pins);
```

### Getting the Public Key Hash

```bash
# From a live server
openssl s_client -connect api.yourschool.lk:443 -servername api.yourschool.lk 2>/dev/null \
  | openssl x509 -noout -pubkey \
  | openssl pkey -pubin -outform DER \
  | openssl dgst -sha256 -binary \
  | base64

# From a certificate file
openssl x509 -in cert.pem -noout -pubkey \
  | openssl pkey -pubin -outform DER \
  | openssl dgst -sha256 -binary \
  | base64
```

### Pinning Strategy and Rotation

```
Always pin at least 2 hashes:
  → Current certificate's public key
  → Backup certificate's public key (generated in advance, not yet active)

When rotating certificates:
  1. Generate new key pair → get new certificate
  2. Ship app update with: [old key hash, new key hash] pinned
  3. Wait for old app versions to update (monitor adoption rate)
  4. Deploy new certificate on server
  5. Old cert now invalid, new cert matches new hash → connections work
  6. After enough time: remove old hash from new app versions

Pinning mismatch → app cannot reach API → user sees "connection failed"
This is severe — rushed key rotation without updating pins = production outage
```

### When NOT to Pin

Pinning adds operational complexity:

- Certificate rotation becomes a coordination effort (app release required)
- Some corporate networks use TLS inspection → pinning breaks for those users
- Wrong for apps targeting enterprise/MDM environments

For Paideon: pinning is appropriate for the parent/student-facing mobile app where you control updates and the API is a known, stable domain.

---

## 4. Biometric Authentication on Mobile

Mobile biometrics (fingerprint, face) authenticate locally on the device — they do NOT send biometric data to your server.

```
Flow:
  1. User sets up biometrics in your app
  2. App creates a key in the device Keystore/Keychain, protected by biometrics
  3. The key is associated with a refresh token or cryptographic credential

  4. User opens app → biometric prompt
  5. Successful biometric → OS unlocks the key → app reads refresh token
  6. App uses refresh token to get new access token from server
  7. Server validates refresh token → issues access token → session restored
```

```ts
// React Native — biometric-protected access token retrieval
import * as LocalAuthentication from "expo-local-authentication";
import * as Keychain from "react-native-keychain";

async function authenticateWithBiometrics(): Promise<string | null> {
  // Check if biometrics are available
  const hasHardware = await LocalAuthentication.hasHardwareAsync();
  const isEnrolled = await LocalAuthentication.isEnrolledAsync();
  
  if (!hasHardware || !isEnrolled) {
    // Fall back to PIN/password
    return null;
  }

  const result = await LocalAuthentication.authenticateAsync({
    promptMessage: "Authenticate to access the app",
    cancelLabel: "Use PIN instead",
    fallbackLabel: "Use PIN",
  });

  if (!result.success) return null;

  // Biometric authenticated → read the protected credential
  const credentials = await Keychain.getGenericPassword({
    authenticationPrompt: { title: "Authenticating" },
  });
  
  return credentials ? credentials.password : null;
}
```

### What Biometrics Protect Against

```
Protects against:
  → Someone finding your unlocked phone and opening the app
  → Shoulder surfing PIN entry

Does NOT protect against:
  → A coerced finger/face (legal/physical coercion)
  → Sophisticated bypass on jailbroken devices (Frida hooks)
  → If device is already unlocked, some implementations are bypassed

The security model: biometrics protect local access, not remote access.
The server still validates the token — biometrics just gate the local token retrieval.
```

---

## 5. Jailbreak and Root Detection

Jailbroken iOS / rooted Android devices have weakened sandboxes — apps can be inspected, modified, and their traffic intercepted more easily.

### Detection Techniques

```ts
// react-native-jail-monkey or similar libraries
import JailMonkey from "jail-monkey";

async function checkDeviceSecurity(): Promise<boolean> {
  if (JailMonkey.isJailBroken()) {
    // Device is jailbroken/rooted
    return false;
  }
  if (JailMonkey.canMockLocation()) {
    // Location can be faked (weak signal but worth noting)
  }
  if (JailMonkey.isDebuggedMode()) {
    // App is being debugged (attached debugger)
    return false;
  }
  return true;
}
```

### What to Do When Detected

```
Options (most to least aggressive):
  1. Refuse to run: "This app cannot run on jailbroken devices"
  2. Refuse to store sensitive data: disable biometric auth, no offline data
  3. Log and alert: allow operation but flag for admin review
  4. Do nothing: accept the risk

For a school management system:
  Mild response: warn the user, disable offline data storage, log to server
  Refusing to run entirely is too aggressive for an educational app
  The primary defense is still server-side — a jailbroken device doesn't bypass server auth
```

### Bypass Techniques

Attackers can bypass detection using Frida (dynamic instrumentation):

```
Frida: "JavaScript engine injected into any process"
  → Hook JailMonkey.isJailBroken() → always return false
  → Intercept Keychain reads → extract tokens
  → Modify network responses → inject test data
```

Detection is a speed bump, not a wall. Never rely on mobile-side security as the primary control — the backend must enforce all security.

---

## 6. Binary Reverse Engineering

Your mobile app binary can be decompiled and analyzed. Assume anything in the binary can be read by a determined attacker.

```
Android APK:
  jadx, apktool → decompile to readable Java/Kotlin
  Strings tool → extract all hardcoded strings from the binary

iOS IPA:
  Hopper, Ghidra, class-dump → analyze Objective-C/Swift classes
  strings → extract hardcoded values

What attackers look for:
  → Hardcoded API keys, tokens, secrets
  → API endpoints (especially internal/admin ones)
  → Business logic that can be bypassed
  → Crypto keys used for local data encryption
```

### What NOT to Put in the Binary

```
NEVER hardcode in the binary:
  → API keys (AWS, Stripe, etc.) → use your own backend as proxy
  → Admin credentials
  → Encryption keys for server-side data
  → Internal API endpoints that should stay private

Acceptable in the binary:
  → Your public API URL (it has to be there)
  → Certificate pin hashes (the private key never leaves the server)
  → App configuration values (not secrets)
```

### The Mobile Backend Proxy Pattern

```
WRONG: Mobile app talks directly to AWS S3
  The app needs AWS credentials → credentials in the binary → extractable

CORRECT: Mobile app → your backend → AWS S3
  Mobile app: sends request to your API (authenticated with user JWT)
  Your backend: validates auth, generates signed S3 URL, returns to app
  Mobile app: uploads directly to S3 using the time-limited signed URL
  AWS credentials: only on your server, never in the binary
```

---

## 7. Mobile-Specific API Security

### App Attestation

iOS DeviceCheck and Android Play Integrity API let your server verify that a request comes from a genuine, unmodified version of your app — not a modified binary or Frida-hooked process.

```
Flow:
  App → Apple/Google: "Give me an attestation token for this device/app"
  Apple/Google verifies app integrity → returns signed attestation token
  App → Your server: "Here's my attestation token"
  Your server → Apple/Google: "Is this token valid?"
  Apple/Google: "Yes, from app ID X, version Y, on genuine hardware"

This makes it much harder to use modified apps or automated bots.
Relevant for: preventing grade manipulation bots, exam answer extraction.
```

### API Versioning for Mobile Clients

Mobile app updates are not instant — users may be on old versions for months.

```
Always version your API: /api/v1, /api/v2
Support old versions during active mobile clients
Deprecation path:
  v1 deprecated → return deprecation header
  Minimum version enforcement: if app version < X.Y.Z → return 410 Gone
  Force update prompt in app when version is below minimum
```

---

## 8. Deep Links and URL Scheme Security

Mobile apps register URL schemes and universal links for deep linking.

```
Custom URL scheme:  myschoolapp://reset-password?token=abc
Universal link:     https://yourschool.lk/app/reset-password?token=abc
```

### URL Scheme Hijacking

```
Any app can register a custom URL scheme.
Two apps claim: myschoolapp://

When your app calls myschoolapp://reset-password:
  iOS/Android may open the WRONG app
  Attacker app intercepts the reset token

Defense: use Universal Links (iOS) / App Links (Android)
  → Validated via a server-side file: yourschool.lk/apple-app-site-association
  → Only apps with verified domain association can handle the links
  → No third-party app can intercept
```

### Token Security in Deep Links

```
Password reset via deep link:
  yourschool.lk/reset-password?token=abc123

Problems:
  → Token in URL → in system logs, browser history
  → URL shared accidentally → token shared

Better approach:
  Short-lived token in URL → just a pointer
  Token contains no actual permissions itself — just indexes into server state
  Server validates token, returns a one-time session for the reset flow
  Token used once → immediately invalidated
```

---

## 9. Data in Transit and at Rest

### In Transit

```
All API communication: HTTPS (TLS 1.2+)
All WebSocket communication: wss://
Certificate pinning for sensitive endpoints
ATS (App Transport Security) — iOS enforces HTTPS for all connections by default
  Don't add ATS exceptions unless absolutely necessary for legacy integrations
```

### At Rest — On Device

```
Keychain/Keystore: tokens, private keys
  kSecAttrAccessibleWhenUnlockedThisDeviceOnly (iOS)
  BIOMETRY_CURRENT_SET for sensitive credentials

Local database (SQLite via React Native):
  react-native-encrypted-database or expo-sqlite with encryption
  SQLCipher: AES-256 encrypted SQLite

Temporary files:
  Cache directory (cleared by OS under storage pressure)
  Not Documents directory (user-accessible, backed up to iCloud/Google)

Files to exclude from backup:
  Tokens, private keys, sensitive user data
  iOS: set NSURLIsExcludedFromBackupKey on sensitive files
  Android: noBackup flag on files
```

---

## Summary

```
Mobile threat model:
  App binary in attacker's hands — can be decompiled
  Traffic can be proxied on non-pinned apps
  Jailbroken/rooted devices weaken OS sandbox
  Backend must enforce all security — mobile is just a UI

Secure storage:
  iOS: Keychain with WHEN_UNLOCKED_THIS_DEVICE_ONLY
  Android: Keystore + EncryptedSharedPreferences
  Never: AsyncStorage, regular SharedPreferences, localStorage
  Access tokens in memory; refresh tokens in Keychain/Keystore

Certificate pinning:
  Hardcode public key hash in app
  Always pin 2 hashes (current + backup for rotation)
  Coordinate rotation with app update
  Not for enterprise/MDM environments

Biometrics:
  Local authentication gate for token retrieval
  Biometric data never leaves the device
  Server still validates the token — biometrics don't bypass server auth

Jailbreak detection:
  Speed bump, not wall — Frida can bypass it
  Respond by restricting sensitive operations, not necessarily blocking entirely

Binary security:
  No secrets in binary — use backend proxy for external service credentials
  Certificate pin hashes are fine (private key is on server)

App attestation (DeviceCheck, Play Integrity):
  Verify requests come from genuine, unmodified app builds
  Relevant for preventing automated manipulation
```