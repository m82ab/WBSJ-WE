# وضعیت Build فاز صفر

- سورس Native Qt 6 / QML ایجاد شده است.
- سورس سرویس جداگانه ویندوز و ورودی Windows Service Control Manager ایجاد شده است.
- پروتکل IPC نسخه ۱ و کلاینت آن ایجاد شده است.
- اسکریپت Build ویندوز و قالب Inno Setup اضافه شده‌اند.
- آزمون‌های ایستای پروژه اجرا شده‌اند و موفق بوده‌اند.

**Build واقعی هنوز انجام نشده است.** این محیط فاقد Qt SDK و Windows/MSVC است؛ بنابراین فایل EXE و Setup این فاز تولید یا تست نشده‌اند و عمداً از فایل‌های اجرایی نسخه قبلی به‌عنوان خروجی Native استفاده نشده است.

## به‌روزرسانی 2026-11-10

- مسیر توقف Named Pipe Thread اصلاح شد تا به `QThread::nativeHandle()` وابسته نباشد و با Qt 6.5+ سازگارتر باشد؛ توقف I/O هم‌زمان از طریق handle کپی‌شده Win32 انجام می‌شود.
- Workflow ساخت ابری ویندوز در `.github/workflows/windows-build.yml` اضافه شده است. این مسیر از Runner ویندوزی GitHub و Visual Studio 2022 آماده استفاده می‌کند؛ بنابراین نصب Visual Studio روی رایانه کاربر لازم نیست.
- راهنمای اجرای Build ابری در `docs/CLOUD_BUILD_FA.md` قرار دارد.
- Workflow هنوز اجرا نشده است؛ در نتیجه Build و تست واقعی Windows همچنان تأیید نشده‌اند.

## Build محلی (در صورت تمایل)

برای Build محلی باید روی Windows 10/11، Qt 6.5+ برای MSVC 2022، Visual Studio 2022 با Desktop development with C++، Windows SDK، CMake و Inno Setup نصب باشد. سپس در PowerShell:

```powershell
$env:Qt6_DIR = 'C:\Qt\6.8.3\msvc2022_64\lib\cmake\Qt6'
.\scripts\build-windows.ps1
```

پس از Build، قالب `packaging/windows/WBSJWE.iss` را با Inno Setup Compiler بسازید.
