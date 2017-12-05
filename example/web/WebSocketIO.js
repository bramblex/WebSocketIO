
class WebSocketIO extends EventEmitter3 {

    constructor({ namespace = "ws", host = "localhost", port = 8000 }) {
        super();
        this.namespace = namespace
        this.host = host
        this.port = port
        this.connect()
    }

    connect() {
        this.ws = new WebSocket(`ws://${this.host}:${this.port}/${this.namespace}`);

        this.ws.onopen = event => {
            this.emit("connection");
        }

        this.ws.onmessage = event => {
            try {
                const message = JSON.parse(event.data)
                if (message.event) {
                    this.emit(message.event, message.data)
                }
            } catch (err) {
                console.warn('[warn] message parse failed: ' + event.data)
            }
        }

        this.ws.onclose = event => {
            this.emit("disconnect")
        }
    }

    send(event, data) {
        this.ws.send(JSON.stringify({ event, data }))
    }

    close() {
        this.ws.close();
    }

}