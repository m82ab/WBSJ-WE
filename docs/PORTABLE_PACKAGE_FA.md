# بسته اجرایی همراه Runtime

بسته ZIP خروجی CI شامل فایل‌های برنامه و Runtimeهای Qt است. این بسته را می‌توان در یک پوشه Extract کرد؛ اما چون Phase 0 سرویس Windows SCM جداگانه دارد، اجرای آن به‌تنهایی سرویس را ثبت نمی‌کند.

برای ثبت سرویس:

1. PowerShell را با Run as administrator باز کنید.
2. به پوشه Extract شده بروید.
3. دستور زیر را اجرا کنید:

```powershell
.\scripts\install-service.ps1 -ServiceExe .\WBSJ_WE_RecordingService.exe
```

برای حذف سرویس:

```powershell
.\scripts\uninstall-service.ps1
```

پس از نصب سرویس، `WBSJ_WE.exe` را اجرا کنید. حذف سرویس به معنی حذف خودکار پوشه داده‌ها یا لاگ‌ها نیست؛ قبل از پاک‌کردن داده‌ها نسخه پشتیبان بگیرید.
