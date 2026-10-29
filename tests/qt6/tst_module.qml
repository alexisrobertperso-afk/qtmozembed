import QtQuick
import QtTest

Item {
    id: root

    TestCase {
        name: "RedNetWebModule"

        function test_typesAreRegistered() {
            const component = Qt.createQmlObject(
                "import QtQuick; import RedNet.Web; Component { WebView {} }", root)
            compare(component.status, Component.Ready, component.errorString())
            const engine = Qt.createQmlObject(
                "import QtQuick; import RedNet.Web; QtObject { property var engine: WebEngine; property var settings: WebSettings }", root)
            verify(engine.engine !== null && engine.engine !== undefined)
            verify(engine.settings !== null && engine.settings !== undefined)
        }
    }
}
