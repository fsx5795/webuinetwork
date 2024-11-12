document.addEventListener('DOMContentLoaded', () => {
    const select = document.querySelector('select')
    if (typeof webui !== 'undefined') {
        webui.setEventCallback(async (e) => {
            if (e == webui.event.CONNECTED) {
                const ips = await webui.getIps()
                const strList = ips.split(',')
                for (let i in strList)
                    select.options.add(new Option(strList[i]))
            }
        })
    }
    const btn = document.querySelector('button')
    btn.addEventListener('click', () => {
        console.log(select.value)
        const port = document.getElementById('port')
        console.log(port.value)
        const radios = document.getElementsByName('network')
        let type
        for (let i in radios) {
            if (radios[i].checked == true) {
                type = radios[i].value
                break
            }
        }
        webui.startNetwork(select.value, port.value, type)
    })
})
function displayMessage(msg) {
    const textarea = document.querySelector('textarea')
    textarea.value += msg
}
