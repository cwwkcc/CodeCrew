> Your application sends emails — password resets, notifications, invitations. If your email domain isn't secured, attackers can send emails that appear to come from you, phishing your own users with your own domain name. This covers SPF, DKIM, DMARC, and link security in emails.

---

## Table of Contents

1. [Why Email Authentication Exists](#1-why-email-authentication-exists)
2. [SPF — Sender Policy Framework](#2-spf--sender-policy-framework)
3. [DKIM — DomainKeys Identified Mail](#3-dkim--domainkeys-identified-mail)
4. [DMARC — Domain-based Message Authentication](#4-dmarc--domain-based-message-authentication)
5. [Email Authentication in Practice](#5-email-authentication-in-practice)
6. [BIMI — Brand Indicators for Message Identification](#6-bimi--brand-indicators-for-message-identification)
7. [Secure Links in Emails](#7-secure-links-in-emails)
8. [Email Content Security](#8-email-content-security)
9. [Transactional Email Services](#9-transactional-email-services)

---

## 1. Why Email Authentication Exists

SMTP (Simple Mail Transfer Protocol, 1982) was designed in a more trusting era. By default:

```
Anyone can send an email claiming to be from anyone.

SMTP server at attacker.com:
  FROM: noreply@yourschool.lk
  TO: student@gmail.com
  SUBJECT: Reset your password
  BODY: Click here: https://fake-yourschool.lk/reset

Gmail receives it:
  "Who sent this?"
  "attacker.com's SMTP server"
  "Is attacker.com allowed to send emails for yourschool.lk?"
  Without SPF/DKIM: "I don't know" → might deliver it
  With SPF/DKIM: "No, and the signature doesn't match" → spam/reject
```

SPF, DKIM, and DMARC are DNS-based standards that give receiving mail servers a way to verify that an email claiming to be from your domain actually came from an authorized server.

---

## 2. SPF — Sender Policy Framework

SPF publishes a list of IP addresses/servers authorized to send email for your domain. The receiving server checks if the sending server's IP is in that list.

### How It Works

```
1. You publish a DNS TXT record for yourschool.lk:
   "v=spf1 include:amazonses.com include:sendgrid.net ~all"

   This says: "Only Amazon SES and Sendgrid are authorized to send email for yourschool.lk"

2. An email arrives claiming: FROM: noreply@yourschool.lk
   Sent from: attacker.com (IP: 1.2.3.4)

3. Receiving server checks DNS:
   "Is 1.2.3.4 in yourschool.lk's SPF record?"
   → No
   → SPF FAIL

4. An email arrives from Amazon SES (IP: 54.240.x.x)
   "Is 54.240.x.x authorized?"
   → Yes (included via include:amazonses.com)
   → SPF PASS
```

### SPF Record Syntax

```
v=spf1                       # version (required)
ip4:203.0.113.10             # authorize specific IPv4
ip6:2001:db8::/32            # authorize IPv6 range
include:amazonses.com        # authorize all IPs in amazonses.com's SPF record
a                            # authorize the domain's own A record
mx                           # authorize the domain's own MX servers
~all                         # softfail anything else (deliver but mark as suspicious)
-all                         # hardfail anything else (reject)
?all                         # neutral (no policy) — effectively useless for security
```

### The -all vs ~all Debate

```
-all (hardfail): reject emails from unauthorized senders
  → Stronger security
  → Risk: legitimate emails from unknown servers (third-party tools) get rejected
  → Use when you're confident you've listed ALL sending sources

~all (softfail): deliver but mark as suspicious
  → More permissive
  → Safer during setup/migration when you might not know all sending sources
  → Most DMARC policies handle the enforcement anyway
```

### SPF Limitations

- SPF only checks the **envelope From** (the technical SMTP sender) — not the **header From** (what users see in their email client). Attackers can pass SPF while spoofing the visible From.
- DMARC alignment requirement closes this gap.
- SPF has a **DNS lookup limit of 10** — too many `include:` directives → SPF breaks.

---

## 3. DKIM — DomainKeys Identified Mail

DKIM adds a cryptographic signature to outgoing emails. The receiving server can verify the signature using a public key published in DNS.

### How It Works

```
1. You (or your email provider) generate an RSA key pair
   Public key: published as DNS TXT record
   Private key: stored securely on your mail server / email provider

2. When sending an email, the mail server:
   → Selects specific headers + body to sign
   → Computes SHA-256 hash of selected content
   → Signs hash with private key
   → Adds DKIM-Signature header to the email

3. Receiving server:
   → Reads DKIM-Signature header → extracts selector and domain
   → Fetches public key from DNS: selector._domainkey.yourschool.lk
   → Verifies signature
   → Checks that signed headers + body haven't been modified
```

### DKIM Header

```
DKIM-Signature: v=1; a=rsa-sha256; c=relaxed/relaxed;
  d=yourschool.lk;              ← your domain
  s=ses-key-1;                  ← selector (which key to look up)
  h=from:to:subject:date;       ← which headers are signed
  bh=base64(SHA256(body));      ← body hash
  b=base64(signature)           ← the signature itself
```

DNS lookup: `ses-key-1._domainkey.yourschool.lk TXT` Returns the public key that verifies the signature.

### What DKIM Protects

```
DKIM proves: this email was signed by someone with the private key for yourschool.lk
             AND the signed headers/body haven't been modified in transit

DKIM does NOT prove:
  → The sender's identity beyond key possession
  → That the email wasn't forwarded (forwarding usually breaks DKIM)
  → Header From matches the d= domain (DMARC handles this)
```

### Key Rotation

Rotate DKIM keys periodically (every 6-12 months):

```
1. Generate new key pair (new selector: ses-key-2)
2. Publish new public key in DNS: ses-key-2._domainkey.yourschool.lk
3. Configure mail server to sign with new key
4. Wait for TTL to expire on old key's DNS record
5. Remove old key from DNS
```

Old emails signed with the old key remain verifiable until you remove the old DNS record.

---

## 4. DMARC — Domain-based Message Authentication

DMARC ties SPF and DKIM together and tells receiving servers what to do with failures. It also provides reporting.

### How It Works

```
1. You publish:
   _dmarc.yourschool.lk TXT "v=DMARC1; p=quarantine; rua=mailto:dmarc@yourschool.lk"

2. Receiving server processes an email:
   → Checks SPF: pass or fail?
   → Checks DKIM: pass or fail?
   → Checks DMARC alignment:
      Does the SPF "envelope from" domain align with the header From domain?
      Does the DKIM d= domain align with the header From domain?

3. DMARC pass requires:
   SPF passes AND is aligned (envelope from = yourschool.lk)
   OR
   DKIM passes AND is aligned (d= yourschool.lk)

4. If DMARC fails → apply the policy (p=)
```

### DMARC Policy Values

```
p=none:         monitor mode — do nothing, just report
                Use first: gather reports, understand your email landscape

p=quarantine:   put failing emails in spam/junk
                Good intermediate step

p=reject:       reject failing emails outright — never delivered
                Maximum protection — use when confident
```

### DMARC Record Syntax

```
v=DMARC1;        # version (required)
p=reject;        # policy for your domain
sp=quarantine;   # policy for subdomains (optional)
pct=100;         # percentage of emails to apply policy to (100 = all)
rua=mailto:dmarc-aggregate@yourschool.lk;  # aggregate report destination
ruf=mailto:dmarc-forensic@yourschool.lk;   # forensic report destination
adkim=s;         # DKIM alignment: strict (s) or relaxed (r)
aspf=s;          # SPF alignment: strict (s) or relaxed (r)
```

### Alignment

```
Relaxed alignment (default):
  Header From:  alice@mail.yourschool.lk
  DKIM d=:      yourschool.lk           ← subdomain alignment OK in relaxed mode

Strict alignment:
  Header From:  alice@yourschool.lk
  DKIM d=:      yourschool.lk           ← must be exact match

Strict alignment closes subdomain spoofing but can break legitimate subdomain senders.
Start with relaxed, move to strict when you understand your email flows.
```

### DMARC Deployment Path

```
Step 1: p=none; rua=mailto:dmarc-reports@yourschool.lk
  → Monitor for 2-4 weeks
  → Receive aggregate reports showing who is sending email for your domain
  → Identify all legitimate sending sources

Step 2: Fix SPF and DKIM for all legitimate senders
  → Add missing include: to SPF
  → Ensure all sending services have DKIM configured

Step 3: p=quarantine; pct=25  (25% of failing emails quarantined)
  → Gradual rollout
  → Monitor for legitimate emails being quarantined

Step 4: p=quarantine; pct=100

Step 5: p=reject; pct=100
  → Maximum protection
  → Unauthorized senders can no longer deliver emails as yourschool.lk
```

---

## 5. Email Authentication in Practice

### For Paideon Using Amazon SES or Resend

```
1. Domain setup:
   Add your domain in SES/Resend
   They provide you with DNS records to add:
     SPF TXT record
     DKIM CNAME records (they manage the keys)
     Optional: DMARC record (you create this yourself)

2. DNS records to add (example for SES):
   # SPF
   yourschool.lk TXT "v=spf1 include:amazonses.com ~all"

   # DKIM (4 CNAME records provided by SES)
   abc123._domainkey.yourschool.lk CNAME abc123.dkim.amazonses.com

   # DMARC
   _dmarc.yourschool.lk TXT "v=DMARC1; p=none; rua=mailto:dmarc@yourschool.lk"

3. Verification: SES verifies DNS records → domain verified → you can send

4. Testing:
   Send a test email to mail-tester.com (gives you a score 1-10)
   Check headers in Gmail: "Show original" → look for Authentication-Results
   Should show: spf=pass, dkim=pass, dmarc=pass
```

### Testing Email Authentication

```bash
# Send test email and check headers in Gmail
# Gmail → three dots → "Show original"
# Look for:
Authentication-Results: mx.google.com;
  spf=pass (domain of yourschool.lk designates 54.240.x.x as permitted sender)
  dkim=pass header.d=yourschool.lk;
  dmarc=pass (policy=reject)

# Mail Tester: https://www.mail-tester.com
# MXToolbox: https://mxtoolbox.com/EmailHeaders.aspx
```

---

## 6. BIMI — Brand Indicators for Message Identification

BIMI displays your logo next to emails in supported clients (Gmail, Apple Mail, Yahoo). It requires DMARC at p=quarantine or p=reject.

```
_bimi.yourschool.lk TXT "v=BIMI1; l=https://yourschool.lk/logo.svg; a=https://yourschool.lk/bimi-vmc.pem"

l= → URL to your logo (SVG format, specific requirements)
a= → URL to your Verified Mark Certificate (VMC) — paid, optional

Without VMC: some clients show logo, others don't
With VMC: verified checkmark in Gmail (requires brand trademark)
```

BIMI is primarily a brand trust signal — it tells users "this email is authenticated and genuinely from this company." For a school system, it reinforces email legitimacy for parents and students.

---

## 7. Secure Links in Emails

Email links are high-value attack targets. Password reset links, invitation links, and action confirmation links must be secured.

### Token Properties for Email Links

```
Password reset token:
  → Cryptographically random: crypto.randomBytes(32).toString("hex")
  → Short-lived: expires in 15-60 minutes
  → Single-use: invalidated after first use
  → Stored as hash: SHA-256(token) in DB — not the raw token
  → Bound to the user: verified that token belongs to the requesting user

Invitation token:
  → Cryptographically random
  → Expires in 7 days (longer is acceptable for invitations)
  → Single-use: activate once, mark as used
  → Bound to the invited email: token can only be used with the invited address
```

### Implementation

```ts
// Generate and send password reset link
async sendPasswordReset(email: string): Promise<void> {
  const user = await this.findByEmail(email);

  // Don't reveal whether email exists — always return success
  if (!user) return;

  const rawToken = crypto.randomBytes(32).toString("hex");
  const tokenHash = crypto.createHash("sha256").update(rawToken).digest("hex");

  await this.db.passwordResetToken.create({
    data: {
      userId: user.id,
      tokenHash,
      expiresAt: new Date(Date.now() + 15 * 60 * 1000), // 15 minutes
    },
  });

  const resetUrl = `${process.env.APP_URL}/reset-password?token=${rawToken}`;
  await this.emailService.send({
    to: email,
    subject: "Reset your password",
    text: `Click here to reset your password: ${resetUrl}\n\nThis link expires in 15 minutes.`,
  });
}

// Verify and use the token
async resetPassword(rawToken: string, newPassword: string): Promise<void> {
  const tokenHash = crypto.createHash("sha256").update(rawToken).digest("hex");

  const record = await this.db.passwordResetToken.findUnique({
    where: { tokenHash },
    include: { user: true },
  });

  if (!record) throw new BadRequestException("Invalid or expired token");
  if (record.usedAt) throw new BadRequestException("Token already used");
  if (record.expiresAt < new Date()) throw new BadRequestException("Token expired");

  // Mark as used FIRST (before password update — prevents race conditions)
  await this.db.passwordResetToken.update({
    where: { id: record.id },
    data: { usedAt: new Date() },
  });

  // Update password
  const hash = await bcrypt.hash(newPassword, 12);
  await this.db.user.update({
    where: { id: record.userId },
    data: { passwordHash: hash },
  });

  // Revoke all existing sessions (password changed → all devices logged out)
  await this.db.refreshToken.updateMany({
    where: { userId: record.userId },
    data: { revokedAt: new Date() },
  });
}
```

### Email Link Security Checklist

```
□ Tokens are cryptographically random (not sequential IDs, not timestamps)
□ Tokens expire (appropriate duration for the use case)
□ Tokens are single-use (invalidated after first click)
□ Tokens stored as hash (not plaintext — even in your own DB)
□ Reset links invalidate old tokens (user requests reset twice → first link stops working)
□ Password change revokes all existing sessions
□ "Token not found" and "Token expired" return the same error message (don't reveal which)
□ Rate limit reset requests per email address
```

---

## 8. Email Content Security

### HTML Email XSS

Email clients vary wildly in how they render HTML. Some older clients (Outlook) use Word's rendering engine. XSS in email has limited impact (no cookies, no localStorage) but can still load tracking pixels or make requests.

```
If you render user-generated content in emails:
  Sanitize HTML before embedding
  DOMPurify (server-side via jsdom) for HTML sanitization
  Or: render user content as plain text only
```

### Phishing Using Your Domain

Even with SPF/DKIM/DMARC, look-alike domains can still phish your users:

```
yourschool.lk         → your domain (protected by DMARC)
yoursch00l.lk         → typosquat (0 instead of o)
yourschool-portal.lk  → subdomain-style attack
yourschooI.lk         → capital I vs lowercase l (looks identical in some fonts)

Defenses:
  → Register common typosquats if feasible
  → Train users: emails will always come from yourschool.lk
  → Include your domain prominently in emails ("This email is from yourschool.lk")
  → Monitor CT logs for similar domain certificate issuance
```

---

## 9. Transactional Email Services

Sending email directly from your server is inadvisable for production:

```
Problems with self-hosted SMTP:
  → Shared IP reputation (your VPS IP might be blacklisted)
  → No built-in bounce/complaint handling
  → DKIM key management is your responsibility
  → Deliverability requires ongoing maintenance
```

**Recommended services for Paideon scale:**

```
Resend (resend.com):
  → Developer-friendly, good Node.js SDK
  → Free tier: 3,000 emails/month
  → React Email integration for templates
  → Automatic SPF/DKIM setup

Amazon SES:
  → Very cheap ($0.10/1000 emails)
  → Excellent deliverability
  → More configuration required
  → AWS ecosystem integration

Postmark:
  → Great deliverability, transactional focus
  → $1.50/1000 emails

SendGrid:
  → Large scale, marketing + transactional
  → More expensive at small scale
```

```ts
// Resend SDK example
import { Resend } from "resend";

const resend = new Resend(process.env.RESEND_API_KEY);

async function sendPasswordReset(to: string, resetUrl: string) {
  await resend.emails.send({
    from: "no-reply@yourschool.lk", // must be verified domain
    to,
    subject: "Reset your password",
    html: `<p>Click <a href="${resetUrl}">here</a> to reset your password.</p>`,
    text: `Reset your password: ${resetUrl}`, // plain text fallback
  });
}
```

---

## Summary

```
SPF: DNS record listing authorized sending IPs
  v=spf1 include:amazonses.com -all
  Prevents unauthorized IPs from sending as your domain
  Limitation: only checks envelope from, not visible From

DKIM: cryptographic signature on outgoing emails
  Private key signs emails, public key published in DNS
  Proves email content hasn't been modified in transit
  Rotate keys every 6-12 months

DMARC: policy + reporting on SPF/DKIM failures
  p=none (monitor) → p=quarantine → p=reject
  Requires alignment between technical sender and visible From
  Aggregate reports show who's sending email for your domain
  Full deployment prevents domain spoofing

Deployment path:
  1. Set p=none + rua report address
  2. Fix SPF/DKIM for all legitimate senders
  3. Escalate to quarantine, then reject

Email links:
  Random tokens, short expiry, single-use, stored as hash
  Revoke all sessions on password change

Use a transactional email service (Resend, SES)
  Don't run your own SMTP in production
```
