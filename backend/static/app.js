const socket = new WebSocket("ws://localhost:8080/ws");

socket.onopen = () => {
    console.log("Connected");
};

socket.onmessage = (event) => {
    console.log("Message:", event.data);
};

function sendMessage() {
    socket.send("Hello from browser");
}