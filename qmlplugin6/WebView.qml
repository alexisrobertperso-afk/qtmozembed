// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

// QtMozEmbed's view with the signals REDNET BROWSER needs.
import QtQuick
import QtQuick.Window
import RedNet.Web

WebViewItem {
    id: view

    // A tab's content process stopped. reason is Gecko's, empty when it gives none.
    signal contentCrashed(string tabId, string reason)

    onRecvAsyncMessageFromTab: (tabId, persistentId, message, data) => {
        if (message === "EmbedLiteChrome:TabCrashed")
            view.contentCrashed(tabId, data && data.reason ? String(data.reason) : "")
    }

    // Gecko lays pages out in CSS pixels: follow the screen's scale factor.
    Binding {
        target: WebSettings
        property: "pixelRatio"
        value: view.Screen.devicePixelRatio
        when: view.Screen.devicePixelRatio > 0
    }

    Component.onCompleted: view.addMessageListener("EmbedLiteChrome:TabCrashed")
}
