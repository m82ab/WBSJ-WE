# WBSJ WE — Phase 0 (Native Desktop Foundation)

**نسخه:** 0.2.0-phase0  
**هدف:** ساخت پوستهٔ Native ویندوز، سرویس مستقل Recording و ارتباط IPC محلی.

> این بسته، شروع بازطراحی Native است و جایگزین بستهٔ وب‌محور 0.1.0 می‌شود. Phase 0 هنوز NVR عملیاتی نیست: در این مرحله هیچ دوربین، Live View یا ضبطی شبیه‌سازی نمی‌شود.

## آنچه در Phase 0 پیاده‌سازی شده

- برنامهٔ Desktop با Qt 6 / QML و پنجرهٔ مستقل ویندوز؛ هیچ مرورگری باز نمی‌شود.
- پردازش جداگانهٔ `WBSJ_WE_RecordingService.exe`.
- اجرای سرویس در Windows Service Control Manager با نام `WBSJWERecordingService`.
- اجرای سرویس در حالت Console برای توسعه و عیب‌یابی (`--console`).
- IPC محلی مبتنی بر QLocalServer / QLocalSocket با نسخه‌بندی پروتکل JSON خطی.
- پیام سلامت سرویس، نسخه، زمان شروع، تعداد دوربین و تعداد Recorder فعال (در Phase 0 صفر واقعی است).
- ثبت لاگ سرویس در `%PROGRAMDATA%\WBSJ WE\logs`.
- دکمه‌های بازکردن مسیر داده، لاگ و پنجره Services ویندوز.
- رابط تیره با سوئیچ زبان نمایشی EN/FA برای صفحهٔ Phase 0.
- اسکریپت ثبت و حذف سرویس ویندوز و قالب Inno Setup.

## مواردی که هنوز پیاده‌سازی نشده‌اند

- ONVIF Discovery و مدیریت دوربین.
- دریافت RTSP URI از Media Profiles و تست واقعی استریم.
- Decode، Live View، Audio و PTZ.
- Recording، segmenting، retention و playback.
- Windows DPAPI برای رمز دوربین‌ها.
- AI، Cloud و P2P.

هیچ‌یک از موارد بالا در این نسخه به‌عنوان قابلیت آماده معرفی نمی‌شود. این‌ها مراحل بعدی هستند.

## نیازمندی Build

- Windows 10/11 x64
- Visual Studio 2022 با C++ Desktop workload و Windows SDK
- CMake 3.24 یا جدیدتر
- Qt 6.5 یا جدیدتر، ماژول‌های Core, Gui, Qml, Quick, QuickControls2, Network و کامپایلر MSVC سازگار
- Inno Setup 6 برای ساخت Setup

## Build روی Windows

در PowerShell با دسترسی معمولی:

```powershell
$env:Qt6_DIR = 'C:\Qt\6.8.3\msvc2022_64\lib\cmake\Qt6'
.\scripts\build-windows.ps1
```

سپس فایل `packaging/windows/WBSJWE.iss` را با Inno Setup Compiler باز و Compile کنید. مسیر خروجی Installer در `dist` قرار می‌گیرد.

## اجرای توسعه‌ای

در Developer PowerShell، بعد از Build:

```powershell
.\build-windows\Release\WBSJ_WE_RecordingService.exe --console
```

در پنجرهٔ دوم:

```powershell
.\build-windows\Release\WBSJ_WE.exe
```

برای تست سرویس واقعی Windows Service، از PowerShell با دسترسی Administrator استفاده کنید:

```powershell
.\scripts\install-service.ps1 -ServiceExe .\build-windows\Release\WBSJ_WE_RecordingService.exe
```

حذف سرویس:

```powershell
.\scripts\uninstall-service.ps1
```

## پروتکل IPC فاز صفر

نام کانال: `WBSJ_WE_RecordingService_v1`  
نسخه پروتکل: `1`  
قالب: هر درخواست و پاسخ یک JSON در یک خط و با `\n` پایان می‌یابد.

درخواست سلامت:

```json
{"protocol":1,"command":"status"}
```

در Phase 0 فقط `status` و `ping` مجاز هستند. هیچ فرمانی برای شروع/توقف ضبط یا تغییر تنظیمات از IPC ارائه نشده است. در Windows کانال وضعیت برای دسترسی محلی باز است و چون فقط اطلاعات سلامت غیرحساس می‌دهد، هیچ عملیات مدیریتی روی آن قرار نگرفته است. قبل از افزودن API مدیریتی باید ACL اختصاصی Named Pipe، احراز هویت و کنترل دسترسی پیاده‌سازی شود.

## مسیرهای داده

Windows:

```text
%PROGRAMDATA%\WBSJ WE\
    logs\recording-service.log
```

## تست و محدودیت محیط فعلی

این محیط توسعه فاقد Qt 6 و کتابخانه‌های توسعه FFmpeg است؛ بنابراین در اینجا امکان Build واقعی Qt یا تولید و تست EXE ویندوز وجود ندارد. اسکریپت Build برای سیستم Windows دارای Qt/MSVC آماده شده است. تا وقتی Build و تست روی Windows واقعی انجام نشود، این سورس را نباید فایل نصب آماده یا نسخهٔ عملیاتی NVR تلقی کرد.

## Copyright

© WBSJ WE — All Rights Reserved  
https://wbsj.ir


## Build ابری ویندوز (بدون نصب Visual Studio روی کامپیوتر شما)

در پوشه `.github/workflows` یک Workflow برای GitHub Actions وجود دارد. این Workflow روی Runner ویندوزی GitHub با Visual Studio 2022، Qt 6.8.3 و Inno Setup برنامه و Installer را می‌سازد. راهنمای مرحله‌به‌مرحله در `docs/CLOUD_BUILD_FA.md` قرار دارد.

تا زمانی که Workflow در GitHub با وضعیت موفق اجرا نشود، فایل EXE و Setup این فاز Build تأییدشده محسوب نمی‌شوند.
