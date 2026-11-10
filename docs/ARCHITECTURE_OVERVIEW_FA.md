# WBSJ WE — معماری فنی پیشنهادی نسخه 1

**وضعیت:** سند طراحی مبنا — هنوز به معنی پیاده‌سازی یا تست محصول نیست  
**هدف:** بازطراحی از پایه برای Windows 10/11 x64 با رابط Desktop Native و موتور ضبط مستقل  
**نام محصول:** WBSJ WE  
**Copyright:** © WBSJ WE — All Rights Reserved  
**Website:** https://wbsj.ir

---

## 1. تصمیم اصلی: Native Desktop، نه Web Dashboard

نسخه قبلی یک Go HTTP server محلی بود که HTML/CSS/JavaScript را در مرورگر پیش‌فرض باز می‌کرد. آن نسخه Native Desktop نبود و نباید پایه محصول نهایی محسوب شود.

نسخه جدید:

- رابط کاربری با Qt 6 / Qt Quick / QML و C++ ساخته می‌شود.
- هیچ مرورگر خارجی، localhost dashboard، Electron یا صفحه وب برای UI اصلی استفاده نمی‌شود.
- `WBSJ WE.exe` فقط رابط گرافیکی را اجرا می‌کند.
- `WBSJRecordingService.exe` سرویس مستقل ویندوز است و پس از Boot، حتی بدون بازشدن UI، ضبط را ادامه می‌دهد.
- رابط گرافیکی با سرویس از طریق Windows Named Pipes و پیام‌های نسخه‌بندی‌شده ارتباط می‌گیرد.
- خطاهای واقعی دوربین، RTSP، ONVIF، دیسک و FFmpeg در Diagnostics و Log قابل مشاهده خواهند بود.

## 2. معماری کلان

```text
WBSJ WE.exe (Qt 6 / QML / C++)
        |
        | Windows Named Pipes (ACL protected)
        v
WBSJRecordingService.exe (Windows Service)
        |
        +-- Device Manager (ONVIF / WS-Discovery)
        +-- Media Engine (FFmpeg libraries / RTSP)
        +-- Recording Scheduler
        +-- Storage & Retention Manager
        +-- Playback & Export
        +-- Event & Alarm Engine
        +-- AI Orchestrator (separate worker process)
        +-- Database / Audit / Configuration
        +-- Cloud & Remote adapters (optional)
```

### اصل‌های معماری

1. Recording از UI و AI مستقل باشد.
2. خرابی یا توقف AI نباید ضبط را متوقف کند.
3. هر دوربین worker و وضعیت مستقل داشته باشد.
4. قطع شبکه باعث Reconnect با Backoff شود، نه Crash سرویس.
5. رمز دوربین‌ها در فایل JSON یا Log به‌صورت متن ساده ذخیره نشود.
6. هیچ قابلیت غیرفعال یا پیاده‌سازی‌نشده به‌عنوان قابلیت عملیاتی نمایش داده نشود.
7. تمام عملیات حساس در Audit Log ثبت شوند.
8. ظرفیت 128 دوربین هدف طراحی است، نه تضمین بدون بررسی Bitrate، شبکه، CPU، GPU و دیسک.

## 3. ساختار پوشه پیشنهادی

```text
WBSJ_WE/
├── CMakeLists.txt
├── CMakePresets.json
├── vcpkg.json
├── cmake/
│   ├── FindFFmpeg.cmake
│   └── WindowsPackaging.cmake
├── apps/
│   ├── desktop/
│   │   ├── main.cpp
│   │   ├── AppController.cpp
│   │   └── qml/
│   │       ├── Main.qml
│   │       ├── components/
│   │       └── pages/
│   └── recording-service/
│       ├── main.cpp
│       ├── WindowsService.cpp
│       └── ServiceHost.cpp
├── core/
│   ├── domain/
│   ├── ipc/
│   ├── config/
│   ├── database/
│   ├── security/
│   ├── logging/
│   └── diagnostics/
├── modules/
│   ├── devices/
│   │   ├── onvif/
│   │   ├── discovery/
│   │   └── profiles/
│   ├── media/
│   │   ├── rtsp/
│   │   ├── ffmpeg/
│   │   ├── decoder/
│   │   └── renderer/
│   ├── recording/
│   ├── storage/
│   ├── playback/
│   ├── events/
│   ├── alarms/
│   ├── analytics/
│   ├── ai/
│   ├── attendance/
│   ├── speed-monitoring/
│   ├── cloud/
│   ├── remote-access/
│   ├── map/
│   ├── reports/
│   └── users/
├── ui/
│   ├── theme/
│   ├── assets/
│   ├── fonts/
│   └── i18n/
│       ├── en_US.ts
│       └── fa_IR.ts
├── models/
│   ├── manifests/
│   └── README.md
├── resources/
│   ├── icons/
│   ├── sounds/
│   └── brand/
├── packaging/
│   └── windows/
│       ├── installer.iss
│       ├── service-install.ps1
│       └── service-remove.ps1
├── scripts/
│   ├── build-windows.ps1
│   ├── test-windows.ps1
│   └── package-release.ps1
├── tests/
│   ├── unit/
│   ├── integration/
│   ├── media-lab/
│   └── fixtures/
└── docs/
    ├── README_FA.md
    ├── README_EN.md
    ├── ARCHITECTURE_FA.md
    ├── INSTALL_FA.md
    ├── CAMERA_SETUP_FA.md
    ├── AI_SETUP_FA.md
    ├── CLOUD_SETUP_FA.md
    └── P2P_SETUP_FA.md
```

## 4. مدیریت دوربین و اتصال واقعی

### روند افزودن دوربین ONVIF

1. انتخاب کارت شبکه و Subnet مجاز برای Discovery.
2. ارسال WS-Discovery روی UDP multicast پورت 3702.
3. دریافت endpoint و اطلاعات سازنده/مدل در صورت ارائه دستگاه.
4. ورود Credentials توسط اپراتور.
5. دریافت Capabilities و Media Profiles.
6. دریافت RTSP URI از `GetStreamUri` برای Profile انتخاب‌شده.
7. اجرای Test Connection و نمایش خطای قابل فهم.
8. نمایش Preview کوتاه و سپس ذخیره دوربین.

### روند افزودن دستی

- IP/Hostname
- پروتکل و RTSP Port
- RTSP Main Stream URI
- RTSP Substream URI اختیاری
- Username / Password
- Transport: TCP یا UDP
- Test Authentication / Test Stream

پورت HTTP/HTTPS مدیریت دوربین از پورت RTSP جداست. مسیر RTSP عمومی و یکسان برای تمام برندها وجود ندارد؛ نباید برای همه دوربین‌ها یک path ثابت فرض شود.

### تغییر IP

تغییر IP واقعی دوربین فقط وقتی نمایش داده می‌شود که دستگاه از عملیات استاندارد ONVIF مربوطه یا API رسمی سازنده پشتیبانی کند. تغییر آدرس ذخیره‌شده در WBSJ با تغییر IP خود دستگاه یکی نیست.

### وضعیت و Diagnostics

برای هر دوربین: `Discovered`, `Configured`, `Connecting`, `Online`, `Recording`, `Reconnecting`, `Authentication Failed`, `Stream Not Found`, `Timeout`, `Unsupported Codec`, `Error`.

Log باید فرمان/پارامترهای غیرحساس، کد خطا و زمان را ثبت کند؛ Password و URI دارای credential نباید ثبت شوند.

## 5. موتور Media و Recording

### ورودی

- RTSP Main Stream برای ضبط
- RTSP Substream برای Live Grid و پردازش AI سبک
- انتخاب RTSP over TCP به‌عنوان پیش‌فرض و UDP به‌صورت گزینه
- Reconnect با Backoff و تشخیص توقف فریم

### ضبط

- در حالت معمول، ویدیو و صدای سازگار بدون Decode/Re-encode کپی شوند تا CPU مصرف نشود.
- قطعه‌های مستقل با زمان قابل تنظیم ساخته شوند؛ مقدار پیش‌فرض پیشنهادی 60 دقیقه است.
- فایل موقت تا تکمیل segment استفاده و سپس نهایی شود.
- Index دیتابیس، زمان شروع/پایان، دوربین، Codec، اندازه و مسیر فایل را نگه دارد.
- Recovery پس از خاموشی ناگهانی، segment ناقص را شناسایی و در صورت امکان ترمیم یا قرنطینه کند.
- ضبط Continuous، Motion، Schedule و Event در مرحله‌های جداگانه فعال شوند.
- Lock/Bookmark از حذف خودکار جلوگیری کند.

### Live View

- فقط کانال‌های قابل مشاهده Decode شوند.
- Substream در Grid و Main Stream در تک‌نمایش/تمام‌صفحه استفاده شود.
- تعداد پنجره‌ها و کیفیت Live با توان واقعی GPU/CPU تنظیم شود.
- 128 دوربین قابل مدیریت است، اما نمایش هم‌زمان 128 جریان Full HD تضمین نمی‌شود.
- صدای هم‌زمان چند دوربین به‌صورت پیش‌فرض پخش نشود؛ انتخاب یک منبع صوتی فعال باشد.

### کتابخانه‌ها و مجوز

FFmpeg از طریق libavformat/libavcodec و کتابخانه‌های مرتبط استفاده می‌شود. نسخه Build باید با پیکربندی LGPL-compatible و بدون اجزای GPL در صورت انتخاب توزیع LGPL ساخته شود؛ مجوزها، اعلان‌ها و تعهدات انتشار/لینک‌دهی باید پیش از بسته‌بندی بررسی شوند. هیچ باینری FFmpeg بدون تعیین نسخه و مجوز وارد Installer نمی‌شود.

## 6. ذخیره‌سازی و Retention

- پشتیبانی از چند Volume و مسیر مستقل برای گروه/دوربین.
- سهمیه بر اساس GB یا درصد Volume.
- حداقل فضای آزاد رزرو.
- حذف قدیمی‌ترین segmentهای تکمیل‌شده به‌صورت FIFO، با رعایت Lock و Evidence Hold.
- Retention بر اساس ساعت، روز یا تعداد روز نگهداری.
- حذف اتمیک از Index و فایل، همراه با Audit.
- هشدار قطع دیسک، Read-only شدن مسیر، خطای نوشتن و کمبود فضا.
- محاسبه مدت نگهداری تقریبی بر اساس Bitrate واقعی و فضای آزاد.

## 7. دیتابیس و امنیت

### دیتابیس

SQLite با WAL برای نسخه تک‌سیستمی و Migration نسخه‌بندی‌شده. در نسخه Multi-site/Enterprise می‌توان PostgreSQL را به‌عنوان سرویس مرکزی اضافه کرد.

### داده‌های اصلی

- Cameras, Profiles, Groups
- RecordingSegments, StorageVolumes, Bookmarks
- Events, Alarms, Incidents
- Users, Roles, Permissions, Sessions
- AI detections and model metadata
- Attendance records
- Speed observations and calibration profiles
- Audit records

### امنیت

- رمزهای دوربین و کلیدهای Cloud با Windows DPAPI یا Windows Credential Manager محافظت شوند.
- IPC با ACL محدود به حساب سرویس و کاربران مجاز.
- RBAC برای Administrator, Supervisor, Operator, Viewer.
- Audit برای Login, Camera changes, Playback, Export, Delete, AI data access و Remote pairing.
- Face embeddings و داده‌های زیست‌سنجی رمزنگاری‌شده، محدود به نقش مجاز و دارای سیاست حذف/Retention باشند.
- Remote access به‌صورت پیش‌فرض خاموش باشد.

## 8. AI و تحلیل ویدیو

AI در Worker مستقل اجرا می‌شود و از طریق صف رویداد با Recording Service ارتباط دارد. ضبط اولویت قطعی دارد.

- ONNX Runtime با CPU Execution Provider و در صورت سازگاری DirectML/CUDA.
- هر مدل Manifest، نسخه، hash، مجوز، ورودی/خروجی و benchmark داشته باشد.
- انتخاب دوربین، FPS پردازش، رزولوشن، حساسیت و سقف منابع برای هر قابلیت.
- عدم وجود مدل معتبر یعنی قابلیت Disabled/Unavailable، نه تولید داده ساختگی.
- پلاک ایران نیازمند دیتاست مجاز و مدل OCR سازگار با ساختار پلاک ایران است.
- Face Recognition نیازمند مدل، کنترل دسترسی، ثبت رضایت/مبنای قانونی و سیاست نگهداری داده زیست‌سنجی است.
- Speed Measurement نیازمند کالیبراسیون هندسی دوربین، سطح/نقاط مرجع و ارزیابی خطا است؛ از تصویر پلاک به‌تنهایی سرعت استخراج نمی‌شود.
- تشخیص رفتار یا آسیب احتمالی فقط هشدار برای بررسی انسانی است و به‌تنهایی اثبات جرم نیست.

## 9. Cloud و Remote Access

- Cloud: S3-Compatible، صف آپلود، Retry، محدودیت پهنای باند، رمزنگاری قبل از Upload و Retention.
- Remote مرحله اول: Tailscale/WireGuard برای شبکه خصوصی و بدون Port Forwarding.
- Remote مرحله بعد: Signaling + ICE/STUN/TURN/WebRTC و Relay اختصاصی با Pairing token کوتاه‌عمر و قابل لغو.
- QR Code فقط حاوی شناسه Pairing و Token موقت باشد؛ هرگز Password دوربین یا Secret دائمی داخل QR قرار نگیرد.

## 10. رابط کاربری

- Qt Quick/QML، بدون Browser/WebView برای رابط اصلی.
- Dark Mode پیش‌فرض و Light Mode.
- English پیش‌فرض و Persian با RTL کامل.
- فونت Inter و Vazirmatn با مجوز مناسب توزیع.
- Sidebar، Topbar، Live Wall، Playback Timeline، Alarm Center، Smart Map، System Health و Settings.
- وضعیت هر قابلیت باید یکی از حالت‌های واقعی `Available`, `Running`, `Disabled`, `Model Missing`, `Unsupported`, `Error` باشد.
- پنجره‌های شناور، Always-on-Top و Multi-Monitor با API پنجره‌های Native ویندوز.

## 11. ظرفیت و برنامه‌ریزی سخت‌افزار

تخمین خام برای 128 دوربین با ضبط 24 ساعته و Bitrate ثابت:

| Bitrate هر دوربین | Bitrate ورودی مجموع | نوشتن تقریبی دیسک | حجم روزانه تقریبی |
|---:|---:|---:|---:|
| 2 Mbps | 256 Mbps | 32 MB/s | 2.76 TB/day |
| 4 Mbps | 512 Mbps | 64 MB/s | 5.53 TB/day |
| 8 Mbps | 1.024 Gbps | 128 MB/s | 11.06 TB/day |

این اعداد تقریبی و بدون سربار شبکه، صوت، تغییر صحنه و نوسان Bitrate هستند. برای 128 دوربین 8Mbps، شبکه 1GbE کافی نیست و 2.5GbE/10GbE و زیرساخت ذخیره‌سازی مناسب لازم می‌شود. CPU/GPU برای Decode و AI جداگانه محاسبه می‌شوند.

## 12. فازهای توسعه و معیار پذیرش

### Phase 0 — Foundation

- Native Qt app و سرویس Windows
- IPC و احراز هویت محلی
- SQLite migrations و DPAPI
- Installer، Uninstaller، logging و crash recovery
- معیار پذیرش: برنامه در پنجره Native باز شود؛ با بستن UI سرویس باقی بماند؛ پس از Restart ضبط آزمایشی ادامه یابد.

### Phase 1 — Core NVR

- ONVIF discovery و manual RTSP
- تست اتصال و نمایش خطای دقیق
- Live single camera و grid با substream
- Recording main stream و segmenting
- Storage quota و FIFO retention
- Playback timeline و export clip
- معیار پذیرش: تست واقعی با حداقل دو مدل/برند دوربین، 24 ساعت ضبط، قطع/وصل شبکه، خاموشی ناگهانی و پرشدن فضای آزمایشی.

### Phase 1.1 — Operations

- Multi-camera layouts, PTZ سازگار، alarms، motion zones، map، system health، bookmarks، video wall.

### Phase 1.2 — AI Analytics

- Person/vehicle, Persian plate OCR, color/type, face (با کنترل‌های حریم خصوصی), line crossing, loitering.

### Phase 1.3 — Forensic & Attendance

- Plate/vehicle search، face search مجاز، attendance، incident management.

### Phase 1.4 — Enterprise

- Speed calibration، cloud, remote client, multi-site, central management, SDK/API.

هر فاز تنها پس از عبور از تست‌های تعریف‌شده Release می‌شود. قابلیت بدون Backend واقعی یا مدل AI نصب‌شده نباید به‌عنوان فعال نمایش داده شود.

## 13. تست و Release

- Unit tests برای parser، storage policy، DB migrations، permissions.
- Integration tests با RTSP test server و ONVIF simulator.
- Hardware-in-the-loop tests با دوربین واقعی.
- Soak test ضبط حداقل 24 تا 72 ساعت.
- Test قطع برق/شبکه، reconnect، disk full، خراب‌شدن segment و restore.
- Performance tests با bitrate و تعداد streamهای مرحله‌ای.
- Build روی Windows 10/11 x64 با MSVC 2022.
- Installer شامل Qt runtime/plugins، کتابخانه‌های لازم FFmpeg، VC++ runtime در صورت نیاز، سرویس، آیکون، مجوزها و Uninstaller.
- امضای دیجیتال فایل‌ها در Release نهایی توصیه می‌شود.

## 14. تصمیم‌های قفل‌شده برای شروع بازنویسی

1. رابط کاربری Native Qt 6/QML است؛ نه مرورگر، نه Electron.
2. هسته و سرویس ضبط C++ هستند.
3. ضبط از رابط گرافیکی مستقل است.
4. ONVIF برای دریافت Stream URI مرجع اصلی است؛ RTSP دستی مسیر جایگزین.
5. Main Stream برای ضبط و Substream برای Live/AI در صورت پشتیبانی.
6. SQLite WAL برای NVR محلی.
7. DPAPI برای حفاظت Credentialها.
8. ابتدا Phase 0 و Phase 1؛ هیچ ماژول AI یا Cloud نمایشی وارد Core Release نمی‌شود.
9. تست واقعی دوربین قبل از اعلام پشتیبانی قطعی الزامی است.
