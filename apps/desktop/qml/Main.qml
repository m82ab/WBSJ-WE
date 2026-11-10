import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Window

ApplicationWindow {
    id: window
    width: 1480
    height: 920
    minimumWidth: 1120
    minimumHeight: 720
    visible: true
    title: "WBSJ WE — Intelligent NVR"
    color: colors.bg

    property bool persian: false
    property string activePage: "overview"
    property var colors: ({
        bg: "#080D16", panel: "#0F1827", panel2: "#121F31", line: "#203149",
        text: "#E8F0FA", muted: "#8294AD", cyan: "#35D0DC", blue: "#398BFF",
        green: "#28C99A", red: "#F06472", amber: "#E9B85C"
    })

    function t(en, fa) { return persian ? fa : en }
    function pageTitle() {
        if (activePage === "overview") return t("Command overview", "نمای کلی مرکز کنترل")
        if (activePage === "service") return t("Service status", "وضعیت سرویس")
        if (activePage === "devices") return t("Device foundation", "زیرساخت دستگاه‌ها")
        return t("Module roadmap", "نقشه راه ماژول‌ها")
    }

    component Panel: Rectangle {
        radius: 14
        color: "#0F1827"
        border.color: "#203149"
        border.width: 1
    }

    component ActionButton: Button {
        id: action
        property color accent: "#35D0DC"
        implicitHeight: 40
        contentItem: Text {
            text: action.text
            color: action.enabled ? "#E8F0FA" : "#8294AD"
            font.pixelSize: 12
            font.weight: Font.DemiBold
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
        }
        background: Rectangle {
            radius: 9
            color: action.down ? "#1A3448" : (action.hovered ? "#162A3D" : "#101E2E")
            border.color: action.enabled ? action.accent : "#203149"
            border.width: 1
        }
    }

    RowLayout {
        anchors.fill: parent
        spacing: 0
        LayoutMirroring.enabled: window.persian
        LayoutMirroring.childrenInherit: true

        Rectangle {
            Layout.preferredWidth: 252
            Layout.fillHeight: true
            color: "#0B1422"
            border.color: colors.line
            border.width: 1

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 18
                spacing: 16

                RowLayout {
                    Layout.fillWidth: true
                    spacing: 12
                    Rectangle {
                        width: 42; height: 42; radius: 12
                        gradient: Gradient {
                            GradientStop { position: 0; color: "#23B7C9" }
                            GradientStop { position: 1; color: "#267BCE" }
                        }
                        Text { anchors.centerIn: parent; text: "◉"; color: "#04131F"; font.pixelSize: 24; font.bold: true }
                    }
                    ColumnLayout {
                        spacing: 1
                        Text { text: "WBSJ WE"; color: colors.text; font.pixelSize: 18; font.bold: true; font.letterSpacing: 1.5 }
                        Text { text: "INTELLIGENT NVR"; color: colors.muted; font.pixelSize: 9; font.letterSpacing: 2 }
                    }
                }

                Rectangle { Layout.fillWidth: true; height: 1; color: colors.line }
                Text { text: t("CONTROL CENTER", "مرکز کنترل"); color: colors.muted; font.pixelSize: 10; font.bold: true; font.letterSpacing: 1.6 }

                Repeater {
                    model: [
                        {id:"overview", icon:"▦", en:"Overview", fa:"نمای کلی", phase:""},
                        {id:"devices", icon:"▣", en:"Devices", fa:"دستگاه‌ها", phase:"P1"},
                        {id:"live", icon:"▻", en:"Live View", fa:"نمایش زنده", phase:"P1"},
                        {id:"recording", icon:"◉", en:"Recording", fa:"ضبط", phase:"P1"},
                        {id:"playback", icon:"◷", en:"Playback", fa:"بازپخش", phase:"P1"},
                        {id:"ai", icon:"✳", en:"AI Analytics", fa:"تحلیل هوشمند", phase:"P2"},
                        {id:"alarms", icon:"♧", en:"Alarm Center", fa:"مرکز هشدار", phase:"P1"},
                        {id:"service", icon:"⌘", en:"Service Health", fa:"سلامت سرویس", phase:""}
                    ]
                    delegate: Button {
                        required property var modelData
                        Layout.fillWidth: true
                        implicitHeight: 44
                        onClicked: window.activePage = (modelData.id === "overview" || modelData.id === "service") ? modelData.id : "roadmap"
                        background: Rectangle {
                            radius: 9
                            color: (window.activePage === modelData.id) ? "#123343" : (parent.hovered ? "#111F30" : "transparent")
                            border.color: window.activePage === modelData.id ? "#1C7180" : "transparent"
                        }
                        contentItem: RowLayout {
                            spacing: 12
                            Text { text: modelData.icon; color: window.activePage === modelData.id ? colors.cyan : colors.muted; font.pixelSize: 18; Layout.preferredWidth: 22; horizontalAlignment: Text.AlignHCenter }
                            Text { text: window.t(modelData.en, modelData.fa); color: window.activePage === modelData.id ? colors.cyan : colors.text; font.pixelSize: 12; Layout.fillWidth: true }
                            Text { text: modelData.phase; color: colors.muted; font.pixelSize: 9; visible: modelData.phase !== "" }
                        }
                    }
                }

                Item { Layout.fillHeight: true }
                Panel {
                    Layout.fillWidth: true
                    implicitHeight: 104
                    ColumnLayout {
                        anchors.fill: parent; anchors.margins: 12; spacing: 7
                        RowLayout {
                            Text { text: "●"; color: backend.serviceState === "Online" ? colors.green : colors.red; font.pixelSize: 11 }
                            Text { text: backend.serviceState === "Online" ? t("SERVICE ONLINE", "سرویس فعال") : t("SERVICE OFFLINE", "سرویس قطع است"); color: colors.text; font.pixelSize: 10; font.bold: true }
                            Item { Layout.fillWidth: true }
                        }
                        Text { text: backend.serviceMessage; color: colors.muted; font.pixelSize: 9; wrapMode: Text.WordWrap; Layout.fillWidth: true; maximumLineCount: 3 }
                    }
                }
                Text { text: "WBSJ WE  •  v" + appVersion; color: colors.muted; font.pixelSize: 9; Layout.alignment: Qt.AlignHCenter }
            }
        }

        ColumnLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: 0

            Rectangle {
                Layout.fillWidth: true
                Layout.preferredHeight: 72
                color: "#0B1422"
                border.color: colors.line
                RowLayout {
                    anchors.fill: parent; anchors.leftMargin: 28; anchors.rightMargin: 28; spacing: 12
                    ColumnLayout {
                        spacing: 2
                        Text { text: window.pageTitle(); color: colors.text; font.pixelSize: 18; font.bold: true }
                        Text { text: t("WBSJ WE / Phase 0 foundation", "WBSJ WE / زیرساخت فاز صفر"); color: colors.muted; font.pixelSize: 10 }
                    }
                    Item { Layout.fillWidth: true }
                    Rectangle {
                        radius: 8; color: "#102235"; border.color: colors.line; implicitWidth: 110; implicitHeight: 34
                        RowLayout {
                            anchors.centerIn: parent; spacing: 7
                            Text { text: "●"; color: backend.serviceState === "Online" ? colors.green : colors.red; font.pixelSize: 9 }
                            Text { text: backend.serviceState; color: colors.text; font.pixelSize: 10 }
                        }
                    }
                    ActionButton { text: window.persian ? "EN" : "FA"; accent: colors.blue; onClicked: window.persian = !window.persian }
                    ActionButton { text: "↻  " + t("Refresh", "بروزرسانی"); onClicked: backend.refresh() }
                }
            }

            Flickable {
                Layout.fillWidth: true
                Layout.fillHeight: true
                clip: true
                contentWidth: availableWidth
                contentHeight: contentColumn.implicitHeight + 60

                ColumnLayout {
                    id: contentColumn
                    width: parent.width - 56
                    x: 28; y: 24
                    spacing: 20

                    RowLayout {
                        Layout.fillWidth: true
                        ColumnLayout {
                            spacing: 4
                            Text { text: window.activePage === "overview" ? t("Good day, Operator", "روز بخیر، اپراتور") : window.pageTitle(); color: colors.text; font.pixelSize: 24; font.bold: true }
                            Text { text: t("A native Windows surveillance platform — foundation build", "پلتفرم نظارت تصویری مستقل ویندوز — نسخه زیرساخت"); color: colors.muted; font.pixelSize: 11 }
                        }
                        Item { Layout.fillWidth: true }
                        Rectangle { radius: 8; color: "#102235"; border.color: colors.line; implicitWidth: 120; implicitHeight: 32
                            Text { anchors.centerIn: parent; text: t("PHASE 0  •  CORE", "فاز صفر  •  هسته"); color: colors.cyan; font.pixelSize: 9; font.bold: true; font.letterSpacing: 1 }
                        }
                    }

                    RowLayout {
                        visible: window.activePage !== "roadmap"
                        Layout.fillWidth: true
                        spacing: 14
                        Repeater {
                            model: [
                                {labelEn:"Configured cameras", labelFa:"دوربین‌های تعریف‌شده", value: backend.configuredCameras, noteEn:"No camera records yet", noteFa:"هنوز دوربینی ثبت نشده", color:"#35D0DC", icon:"▣"},
                                {labelEn:"Active recorders", labelFa:"ضبط‌کننده‌های فعال", value: backend.activeRecorders, noteEn:"Recording engine not in Phase 0", noteFa:"موتور ضبط در فاز صفر فعال نیست", color:"#28C99A", icon:"◉"},
                                {labelEn:"Service version", labelFa:"نسخه سرویس", value: backend.serviceVersion, noteEn:"Local IPC health endpoint", noteFa:"ارتباط محلی با سرویس", color:"#398BFF", icon:"⌘"}
                            ]
                            delegate: Panel {
                                required property var modelData
                                Layout.fillWidth: true
                                Layout.preferredHeight: 126
                                ColumnLayout {
                                    anchors.fill: parent; anchors.margins: 16; spacing: 8
                                    RowLayout {
                                        Text { text: modelData.icon; color: modelData.color; font.pixelSize: 17 }
                                        Item { Layout.fillWidth: true }
                                        Rectangle { width: 6; height: 6; radius: 3; color: modelData.color }
                                    }
                                    Text { text: String(modelData.value); color: colors.text; font.pixelSize: 24; font.bold: true }
                                    Text { text: window.t(modelData.labelEn, modelData.labelFa); color: colors.muted; font.pixelSize: 10 }
                                    Text { text: window.t(modelData.noteEn, modelData.noteFa); color: colors.muted; font.pixelSize: 9; elide: Text.ElideRight; Layout.fillWidth: true }
                                }
                            }
                        }
                    }

                    Panel {
                        visible: window.activePage !== "roadmap"
                        Layout.fillWidth: true
                        implicitHeight: 240
                        ColumnLayout {
                            anchors.fill: parent; anchors.margins: 20; spacing: 12
                            RowLayout {
                                Text { text: t("Recording service", "سرویس ضبط"); color: colors.text; font.pixelSize: 15; font.bold: true }
                                Item { Layout.fillWidth: true }
                                Rectangle { radius: 6; color: backend.serviceState === "Online" ? "#10372F" : "#3B2028"; implicitWidth: 78; implicitHeight: 24
                                    Text { anchors.centerIn: parent; text: backend.serviceState.toUpperCase(); color: backend.serviceState === "Online" ? colors.green : colors.red; font.pixelSize: 9; font.bold: true }
                                }
                            }
                            Rectangle { Layout.fillWidth: true; height: 1; color: colors.line }
                            Text { text: backend.serviceMessage; color: colors.text; font.pixelSize: 12; wrapMode: Text.WordWrap; Layout.fillWidth: true }
                            RowLayout {
                                Layout.fillWidth: true
                                ColumnLayout {
                                    spacing: 3
                                    Text { text: t("SERVICE STARTED", "زمان شروع سرویس"); color: colors.muted; font.pixelSize: 9; font.bold: true }
                                    Text { text: backend.startedAt; color: colors.text; font.pixelSize: 11 }
                                }
                                Item { Layout.fillWidth: true }
                                ActionButton { text: t("Reconnect", "اتصال مجدد"); onClicked: backend.refresh() }
                            }
                        }
                    }

                    RowLayout {
                        visible: window.activePage !== "roadmap"
                        Layout.fillWidth: true
                        spacing: 14
                        Panel {
                            Layout.fillWidth: true
                            Layout.preferredHeight: 192
                            ColumnLayout {
                                anchors.fill: parent; anchors.margins: 18; spacing: 10
                                Text { text: t("Quick actions", "دسترسی سریع"); color: colors.text; font.pixelSize: 14; font.bold: true }
                                ActionButton { Layout.fillWidth: true; text: "▤   " + t("Open service logs", "بازکردن گزارش سرویس"); onClicked: backend.openLogDirectory() }
                                ActionButton { Layout.fillWidth: true; text: "▣   " + t("Open data directory", "بازکردن مسیر داده‌ها"); onClicked: backend.openDataDirectory() }
                                ActionButton { Layout.fillWidth: true; text: "⚙   " + t("Windows Services", "سرویس‌های ویندوز"); onClicked: backend.openWindowsServices() }
                            }
                        }
                        Panel {
                            Layout.fillWidth: true
                            Layout.preferredHeight: 192
                            ColumnLayout {
                                anchors.fill: parent; anchors.margins: 18; spacing: 8
                                Text { text: t("Phase 0 scope", "محدوده فاز صفر"); color: colors.text; font.pixelSize: 14; font.bold: true }
                                Text { text: t("This build establishes the native desktop shell, the independent Windows service process and the local IPC health channel. Camera discovery, RTSP decoding, recording and playback are Phase 1 deliverables.", "این نسخه پوسته دسکتاپ Native، فرایند مستقل سرویس ویندوز و کانال ارتباط محلی را پایه‌گذاری می‌کند. کشف دوربین، رمزگشایی RTSP، ضبط و بازپخش در فاز اول توسعه خواهند بود."); color: colors.muted; font.pixelSize: 10; wrapMode: Text.WordWrap; Layout.fillWidth: true; Layout.fillHeight: true }
                                Text { text: t("No simulated cameras or recording activity are shown.", "هیچ دوربین یا فعالیت ضبط ساختگی نمایش داده نمی‌شود."); color: colors.cyan; font.pixelSize: 10; font.bold: true }
                            }
                        }
                    }

                    Panel {
                        visible: window.activePage === "roadmap"
                        Layout.fillWidth: true
                        implicitHeight: 220
                        ColumnLayout {
                            anchors.fill: parent; anchors.margins: 22; spacing: 12
                            Text { text: t("Not part of Phase 0", "در محدوده فاز صفر نیست"); color: colors.amber; font.pixelSize: 16; font.bold: true }
                            Text { text: t("This module is planned for a later milestone. It is intentionally not connected to mock data or non-functional controls.", "این ماژول برای مرحله بعدی برنامه‌ریزی شده است. عمداً به داده ساختگی یا کنترل‌های بدون عملکرد متصل نشده است."); color: colors.text; font.pixelSize: 12; wrapMode: Text.WordWrap; Layout.fillWidth: true }
                            Text { text: t("The next delivery focuses on real camera onboarding, ONVIF discovery, RTSP connection testing and the first verified live stream.", "تحویل بعدی بر افزودن دوربین واقعی، کشف ONVIF، تست اتصال RTSP و نمایش اولین استریم زنده تأییدشده تمرکز دارد."); color: colors.muted; font.pixelSize: 10; wrapMode: Text.WordWrap; Layout.fillWidth: true }
                        }
                    }

                    Panel {
                        Layout.fillWidth: true
                        implicitHeight: 64
                        RowLayout {
                            anchors.fill: parent; anchors.margins: 16
                            Text { text: "© WBSJ WE — All Rights Reserved"; color: colors.muted; font.pixelSize: 9 }
                            Item { Layout.fillWidth: true }
                            Text {
                                text: "wbsj.ir"; color: colors.cyan; font.pixelSize: 10; font.bold: true
                                MouseArea { anchors.fill: parent; cursorShape: Qt.PointingHandCursor; onClicked: backend.openWebsite() }
                            }
                        }
                    }
                }
            }
        }
    }
}
