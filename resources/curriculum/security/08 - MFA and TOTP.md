> Passwords alone are a single point of failure. Multi-factor authentication requires a second proof of identity — something you have, not just something you know. This covers MFA factor types, how TOTP works mathematically, backup codes, WebAuthn/passkeys, and why SMS auth is weak.

---

## Table of Contents

1. [Why MFA Exists](#1-why-mfa-exists)
2. [MFA Factor Types](#2-mfa-factor-types)
3. [TOTP — Time-Based One-Time Passwords](#3-totp--time-based-one-time-passwords)
4. [HOTP — The Foundation of TOTP](#4-hotp--the-foundation-of-totp)
5. [TOTP Internals — How the 6-Digit Code Is Generated](#5-totp-internals--how-the-6-digit-code-is-generated)
6. [Implementing TOTP](#6-implementing-totp)
7. [Backup Codes](#7-backup-codes)
8. [SMS / Email OTP — Why It's Weak](#8-sms--email-otp--why-its-weak)
9. [WebAuthn and Passkeys](#9-webauthn-and-passkeys)
10. [Biometric Authentication](#10-biometric-authentication)
11. [MFA Bypass Attacks](#11-mfa-bypass-attacks)

---

## 1. Why MFA Exists

```
Single factor (password only):
  Attacker gets your password via:
  → Data breach (other site, reused password)
  → Phishing (you entered it on a fake site)
  → Keylogger
  → Shoulder surfing
  → Brute force (weak password)
  
  Result: account compromised

Multi-factor (password + TOTP):
  Attacker gets your password → still needs your phone
  Attacker has your phone → still needs your password
  Attacker would need BOTH simultaneously → much harder
```

MFA doesn't make accounts unbreakable. It makes them expensive enough to attack that attackers move on to easier targets.

Statistically: MFA prevents ~99% of automated credential stuffing attacks and ~66% of targeted phishing attacks (Google internal study, 2019).

---

## 2. MFA Factor Types

```
Something you KNOW:
  Password, PIN, security question
  Weakest — can be phished, leaked, forgotten

Something you HAVE:
  TOTP app (Google Authenticator, Authy)
  Hardware key (YubiKey, FIDO2)
  Phone (for SMS — but SMS is weak, see below)
  Backup codes
  Stronger — requires physical access

Something you ARE:
  Fingerprint, face recognition, iris scan
  Biometrics — can't be changed if compromised
  Varies in strength depending on implementation
  
Something you DO (behavioral):
  Typing patterns, mouse movements
  Used as passive authentication signals, not primary factors
  
Somewhere you ARE:
  IP geolocation, network location
  Weak — VPNs bypass it
  Useful as risk signal, not primary factor
```

---

## 3. TOTP — Time-Based One-Time Passwords

TOTP (RFC 6238) generates a 6-digit code that:

- Changes every 30 seconds
- Is derived from a shared secret + current time
- Requires no network connection to generate
- Works with any standards-compliant authenticator app

The secret is shared once (during setup via QR code). After that, the user's app and your server independently compute the same code without any communication.

```
Setup:
  Server generates random 160-bit secret
  Secret encoded as base32 and shown as QR code
  User scans with Google Authenticator / Authy / Apple Passwords
  App stores the secret
  Server stores the secret (encrypted in DB)

Login:
  User opens app → app computes 6-digit code from secret + current time
  User enters code
  Server computes the same code from stored secret + current time
  Match → verified → tokens issued
  
  No network request from the app.
  No SMS. No push notification.
  Pure offline computation.
```

---

## 4. HOTP — The Foundation of TOTP

TOTP is built on HOTP (HMAC-based One-Time Password, RFC 4226).

HOTP generates a one-time code from a secret + a counter:

```
HOTP(secret, counter) =
  1. Compute: hmac = HMAC-SHA1(secret, counter as 8-byte big-endian integer)
  2. Dynamic Truncation:
     offset = hmac[19] & 0x0f          (last byte, lower nibble)
     P = hmac[offset..offset+4]         (4 bytes starting at offset)
     P = P & 0x7FFFFFFF                 (clear the MSB — avoid sign issues)
  3. Code = P mod 10^6                  (6-digit code)
     Code = leftpad with zeros to 6 digits

Result: a number 000000 to 999999
```

HOTP uses a counter that increments with each use. Both parties (server and client) must stay in sync on the counter. If the counter drifts (user generates codes without using them), HOTP breaks.

---

## 5. TOTP Internals — How the 6-Digit Code Is Generated

TOTP replaces the counter in HOTP with a time-based counter:

```
T = floor(current_unix_timestamp / 30)
  → Number of 30-second windows since Unix epoch
  → Changes every 30 seconds

TOTP(secret) = HOTP(secret, T)
```

```
Example:
  Current time: 2026-03-11 10:00:00 UTC
  Unix timestamp: 1741691200
  T = floor(1741691200 / 30) = 58056373

  TOTP code = HOTP(secret, 58056373)
  → Both server and user's app compute the same T at the same moment
  → Same secret + same T = same 6-digit code
```

### Clock Drift Tolerance

Real clocks aren't perfectly synchronized. TOTP implementations accept codes from:

- The current window (T)
- The previous window (T-1) — in case the user was entering the code as it changed
- Optionally the next window (T+1)

Most implementations accept T-1, T, T+1 — a 90-second window.

```ts
// Checking current + previous window
const isValid = 
  totp.check(code, secret, { step: 30 }) ||          // current window
  totp.check(code, secret, { step: 30, t: Date.now() / 1000 - 30 }); // previous
```

### Why SHA-1?

HOTP was defined with SHA-1 in 2005. Despite SHA-1 being broken for collision resistance, HMAC-SHA1 is still secure for MACs — the attack against SHA-1 doesn't apply to the HMAC construction. RFC 6238 (TOTP) allows SHA-256 and SHA-512 but SHA-1 is the default for compatibility with authenticator apps.

---

## 6. Implementing TOTP

```ts
import { authenticator } from "otplib";

// Generate secret (during 2FA setup)
const secret = authenticator.generateSecret(); // base32-encoded, 160 bits
// e.g., "JBSWY3DPEHPK3PXP"

// Generate QR code URI (to display as QR for scanning)
const qrUri = authenticator.keyuri(
  "alice@school.lk",  // user identifier (shown in app)
  "School App",        // issuer (shown in app)
  secret
);
// qrUri: "otpauth://totp/School%20App:alice%40school.lk?secret=JBSWY3DPEHPK3PXP&issuer=School%20App"
// Pass this to a QR code library (qrcode package)

// Store secret encrypted in database
const encryptedSecret = encrypt(secret, process.env.TOTP_ENCRYPTION_KEY);
await db.user.update({ where: { id }, data: { totpSecret: encryptedSecret } });

// Verify code during login
const userSecret = decrypt(user.totpSecret, process.env.TOTP_ENCRYPTION_KEY);
authenticator.options = { window: 1 }; // accept T-1, T, T+1
const isValid = authenticator.verify({ token: userCode, secret: userSecret });

// Rate limiting: max 5 attempts per 10 minutes per user
// Lock account after 5 failed TOTP attempts
```

### QR Code Display

```ts
import QRCode from "qrcode";

// Convert URI to QR code image
const qrDataUrl = await QRCode.toDataURL(qrUri);
// Return to frontend: <img src={qrDataUrl} />
// ALSO show the raw secret for manual entry (for users who can't scan)
```

---

## 7. Backup Codes

If a user loses their phone, they need a way to regain access without TOTP.

```
Standard practice:
  → Generate 8-10 random 8-character alphanumeric codes at 2FA setup time
  → Show them ONCE — never again
  → User must download/print/save them immediately
  → Store bcrypt hash of each code in DB

Usage:
  → User enters a backup code instead of TOTP code
  → Server finds matching hash, verifies
  → Mark code as used (one-time — never reusable)
  → Log in AuditLog: BACKUP_CODE_USED
  → Alert admin: backup code was used (potential account recovery or compromise)

After use:
  → If user recovers their phone → reset 2FA → new TOTP secret + new backup codes
  → All codes exhausted → admin manually verifies identity + resets
```

```ts
import crypto from "crypto";
import bcrypt from "bcrypt";

// Generate backup codes
function generateBackupCodes(count = 10): { raw: string[]; hashed: string[] } {
  const raw = Array.from({ length: count }, () =>
    crypto.randomBytes(5).toString("hex").toUpperCase() // "A3B4C5D6E7"
  );
  const hashed = await Promise.all(raw.map(code => bcrypt.hash(code, 10)));
  return { raw, hashed };
}

// Verify backup code
async function verifyBackupCode(userId: string, providedCode: string): Promise<boolean> {
  const codes = await db.backupCode.findMany({
    where: { userId, usedAt: null },
  });

  for (const code of codes) {
    const valid = await bcrypt.compare(providedCode.toUpperCase(), code.hash);
    if (valid) {
      await db.backupCode.update({
        where: { id: code.id },
        data: { usedAt: new Date() },
      });
      await auditLog.log("BACKUP_CODE_USED", userId);
      return true;
    }
  }
  return false;
}
```

---

## 8. SMS / Email OTP — Why It's Weak

SMS OTP sends a one-time code via text message. It's used by many banks and services but has significant weaknesses:

### SIM Swapping

```
Attacker calls carrier pretending to be you:
  "I lost my phone, please transfer my number to this new SIM"
  Carrier verifies with: name, address, last 4 digits of SSN (available in breaches)
  
  Number transferred → attacker's phone receives your SMS
  → Attacker requests password reset → gets SMS code → resets password
  
This attack has been used to steal millions from crypto accounts.
```

### SS7 Protocol Vulnerabilities

The Signaling System 7 (SS7) protocol used by cellular networks has critical vulnerabilities. Nation-state attackers and advanced criminal groups can intercept SMS messages globally.

### Phishing

```
Real-time phishing:
  1. Attacker phishes your password on fake site
  2. Attacker uses password on real site → triggers SMS OTP
  3. Attacker's fake site: "Enter the SMS code you received"
  4. You enter it → attacker submits it on real site → account compromised

TOTP is phishable the same way in real-time, but FIDO2/WebAuthn is not.
```

### Email OTP

Better than SMS (no SIM swapping risk) but worse than TOTP:

- Your email account is often less secure than your phone
- Email delivery is not instant — codes may arrive late, creating a window
- If email account is compromised, so is every service using it for OTP

**When to use SMS/email OTP:** When you need MFA and TOTP adoption would be too high-friction for your user base (e.g., elderly users). It's better than nothing.

**Never use SMS for high-privilege accounts** where SIM swap risk is significant.

---

## 9. WebAuthn and Passkeys

**WebAuthn** (Web Authentication API) is the modern, phishing-resistant MFA standard. It uses public-key cryptography with a hardware authenticator.

### How It Works

```
Registration:
  1. Server sends a challenge (random bytes)
  2. Authenticator (phone, hardware key, TPM) generates a new key pair
  3. Private key stays on authenticator — never leaves
  4. Authenticator signs the challenge with the private key
  5. Public key + signed challenge sent to server
  6. Server stores the public key

Authentication:
  1. Server sends a new challenge
  2. Browser passes challenge to authenticator (requires user gesture: fingerprint, PIN)
  3. Authenticator signs challenge with stored private key
  4. Signature + authenticator data sent to server
  5. Server verifies signature with stored public key
```

### Why WebAuthn Is Phishing-Resistant

The authenticator includes the **origin** (the website's URL) in the signed data. Even if the user is on a phishing site, the signature will be over the phishing site's origin — which won't match the expected origin.

```
User visits: https://bank-ph1shing.com (fake bank)
User's phone signs: { origin: "https://bank-ph1shing.com", challenge: "abc..." }

Server verifies: "Expected origin: https://bank.com. Got: https://bank-ph1shing.com"
→ Verification fails
→ Login rejected
```

No password or TOTP code was ever entered — nothing to steal.

### Passkeys

Passkeys are WebAuthn credentials that can be synced across devices via cloud (iCloud Keychain, Google Password Manager). They replace passwords entirely:

```
No username + password to remember.
Tap your fingerprint → passkey synced to your phone signs the challenge → logged in.

Phishing-resistant by design.
Survives device loss (synced to cloud).
```

Passkeys are being adopted widely (Apple, Google, Microsoft, major banks). For new systems, consider passkeys as the primary auth mechanism rather than passwords.

---

## 10. Biometric Authentication

Biometrics (fingerprint, face) are used as the **user verification step** for WebAuthn — not sent to the server.

```
What happens:
  Your phone has your fingerprint stored in a secure enclave
  When logging in, phone asks for fingerprint to "unlock" the key pair
  The fingerprint never leaves your device
  The server only sees the cryptographic signature

This is crucial: biometrics are stored and verified locally by the device
                 NOT transmitted to the server
                 NOT compared against a database

If a server claims to verify your fingerprint remotely → red flag → never use
```

### Biometric Weaknesses

- **Can't be changed:** If your fingerprint is compromised (e.g., high-res photo of your fingerprints), you can't rotate it like a password
- **False acceptance rate:** Biometric systems have error rates — some accept incorrect biometrics
- **Coercion:** Attacker can physically force you to use your fingerprint

These are acceptable in the context of WebAuthn because the biometric is just unlocking a local key — it's not the primary security control.

---

## 11. MFA Bypass Attacks

### Real-Time Phishing (Attacker-in-the-Middle)

```
Attacker sets up a proxy that looks like the real site.
User enters credentials on fake site.
Attacker relays them to real site in real time.
Real site sends TOTP challenge.
Fake site asks user for TOTP code.
User enters it.
Attacker submits it.

TOTP does NOT prevent this — the code is valid for 30 seconds.
Only WebAuthn prevents this (origin binding).
```

### MFA Fatigue (Push Notification Spam)

```
Used against Okta, Cisco, and others (2022-2023):
  Attacker has password.
  Service uses push notification MFA ("Approve this login?")
  Attacker sends dozens of push notifications.
  User gets notification-spammed at 2 AM.
  User gets frustrated and accidentally/tiredly taps Approve.
  Account compromised.

Mitigation:
  Number matching (user must match a number shown in app)
  Challenge-response (user must type a code)
  Geographic anomaly detection
```

### SS7 Attack on SMS MFA

Already covered above. Use TOTP or WebAuthn instead.

---

## Summary

```
MFA adds a second factor — something you have, not just something you know

TOTP (RFC 6238):
  Secret shared at setup (QR code)
  Code = HOTP(secret, floor(timestamp / 30))
  Changes every 30 seconds
  No network needed
  Accept T-1, T, T+1 windows for clock drift
  Store secret encrypted; store backup codes hashed

SMS OTP weaknesses:
  SIM swapping, SS7 interception, real-time phishing
  Use for low-risk accounts only; never for high-privilege

WebAuthn / Passkeys:
  Public key stored on server, private key never leaves device
  Phishing-resistant: origin binding prevents credential relay
  Passkeys = WebAuthn synced via cloud (replaces passwords)

Biometrics:
  Device-local only — never transmitted to server
  Unlocks WebAuthn key pair
  Can't be changed if compromised

MFA bypass:
  Real-time phishing defeats TOTP but not WebAuthn
  MFA fatigue defeats push notifications
  Use number matching or WebAuthn for high-value accounts
```

---

