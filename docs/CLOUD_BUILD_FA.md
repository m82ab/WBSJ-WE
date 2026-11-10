# ساخت نسخه ویندوز بدون نصب Visual Studio روی کامپیوتر کاربر

برای Build نسخه Native، به کامپایلر MSVC و Qt نیاز است؛ اما لازم نیست این ابزارها روی کامپیوتر شخصی شما نصب شوند. Workflow موجود در `.github/workflows/windows-build.yml` از Windows Server 2022 در GitHub Actions استفاده می‌کند که Visual Studio 2022 و Windows SDK را در Runner دارد و Qt 6.8.3 را در همان محیط نصب می‌کند.

## مراحل لازم

1. یک حساب GitHub بسازید یا وارد حساب خود شوید.
2. یک Repository خصوصی با نام `WBSJ-WE` ایجاد کنید.
3. محتویات این ZIP را Extract کنید.
4. تمام فایل‌ها و پوشه‌های داخل آن را در ریشه Repository قرار دهید. پوشه مخفی `.github` و فایل Workflow آن را حتماً نگه دارید.
5. در GitHub به زبانه Actions بروید و Workflow با نام `WBSJ WE — Windows x64 build` را انتخاب کنید.
6. گزینه Run workflow را بزنید.
7. پس از پایان اجرا، در صفحه همان Workflow بخش Artifacts، فایل `WBSJ-WE-Windows-x64` را دریافت کنید.

Artifact شامل Installer و بسته اجرایی همراه Runtimeهای Qt است. این مسیر نیازمند نصب Visual Studio یا Qt روی کامپیوتر شخصی شما نیست.

## اگر می‌خواهید از رابط وب GitHub استفاده کنید

- در Repository گزینه Add file و سپس Upload files را انتخاب کنید.
- فایل‌ها و پوشه‌های استخراج‌شده را به ریشه Repository اضافه کنید.
- مطمئن شوید `.github/workflows/windows-build.yml` واقعاً در Repository ثبت شده است. چون `.github` پوشه مخفی است، در File Explorer نمایش فایل‌های مخفی را روشن کنید.
- تغییرات را با Commit changes ذخیره کنید.

## محدودیت و صداقت وضعیت

- این Workflow فایل‌ها را در Runner ویندوزی GitHub Build می‌کند؛ تا قبل از اجرای موفق آن، وجود EXE/Setup معتبر تأییدشده نیست.
- اولین Build ممکن است به‌دلیل خطای وابستگی یا کد نیازمند اصلاح باشد؛ خطا باید از Log همان Workflow رفع شود.
- فایل‌های خروجی امضای دیجیتال ناشر ندارند؛ Windows SmartScreen ممکن است هشدار ناشر ناشناس نمایش دهد.
- بسته Portable نیازمند تست روی ویندوز تمیز است؛ وجود DLLهای Qt به‌تنهایی اثبات سازگاری همه سیستم‌ها نیست.
- برای انتشار عمومی، باید گواهی Code Signing معتبر تهیه و به‌صورت GitHub Actions Secret نگهداری شود.
