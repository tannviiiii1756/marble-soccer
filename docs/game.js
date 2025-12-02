const canvas = document.getElementById("gameCanvas");
const ctx = canvas.getContext("2d");

// Game variables
const field = { width: canvas.width, height: canvas.height };
const ball = { x: field.width/2, y: field.height/2, radius: 10, dx: 4, dy: 4 };
const players = {
    red: { x: 50, y: field.height/2, radius: 20, color: "red", dy: 0 },
    blue: { x: field.width-50, y: field.height/2, radius: 20, color: "blue", dy: 0 }
};
let score = { red: 0, blue: 0 };

// Controls
document.addEventListener("keydown", e => {
    if (e.key === "w") players.red.dy = -5;
    if (e.key === "s") players.red.dy = 5;
    if (e.key === "ArrowUp") players.blue.dy = -5;
    if (e.key === "ArrowDown") players.blue.dy = 5;
});
document.addEventListener("keyup", e => {
    if (["w","s"].includes(e.key)) players.red.dy = 0;
    if (["ArrowUp","ArrowDown"].includes(e.key)) players.blue.dy = 0;
});

// Draw functions
function drawCircle(x, y, radius, color) {
    ctx.fillStyle = color;
    ctx.beginPath();
    ctx.arc(x, y, radius, 0, Math.PI*2);
    ctx.fill();
}

function drawField() {
    ctx.fillStyle = "#0a6";
    ctx.fillRect(0, 0, field.width, field.height);
    ctx.strokeStyle = "#fff";
    ctx.lineWidth = 3;
    ctx.strokeRect(0, 0, field.width, field.height);
}

// Collision detection
function checkCollision(player) {
    const dx = ball.x - player.x;
    const dy = ball.y - player.y;
    const distance = Math.sqrt(dx*dx + dy*dy);
    if (distance < ball.radius + player.radius) {
        // simple bounce
        const angle = Math.atan2(dy, dx);
        ball.dx = 5 * Math.cos(angle);
        ball.dy = 5 * Math.sin(angle);
    }
}

// Reset ball after goal
function resetBall() {
    ball.x = field.width/2;
    ball.y = field.height/2;
    ball.dx = (Math.random() > 0.5 ? 4 : -4);
    ball.dy = (Math.random() > 0.5 ? 4 : -4);
}

// Update
function update() {
    // Move players
    players.red.y += players.red.dy;
    players.blue.y += players.blue.dy;

    // Keep players inside field
    players.red.y = Math.max(players.red.radius, Math.min(field.height - players.red.radius, players.red.y));
    players.blue.y = Math.max(players.blue.radius, Math.min(field.height - players.blue.radius, players.blue.y));

    // Move ball
    ball.x += ball.dx;
    ball.y += ball.dy;

    // Ball collisions with walls
    if (ball.y - ball.radius < 0 || ball.y + ball.radius > field.height) {
        ball.dy = -ball.dy;
    }

    // Ball collisions with players
    checkCollision(players.red);
    checkCollision(players.blue);

    // Check goals
    if (ball.x - ball.radius < 0) { score.blue++; resetBall(); }
    if (ball.x + ball.radius > field.width) { score.red++; resetBall(); }

    // Update score display
    document.getElementById("score").innerText = `Red: ${score.red} | Blue: ${score.blue}`;
}

// Draw
function draw() {
    drawField();
    drawCircle(ball.x, ball.y, ball.radius, "white");
    drawCircle(players.red.x, players.red.y, players.red.radius, players.red.color);
    drawCircle(players.blue.x, players.blue.y, players.blue.radius, players.blue.color);
}

// Game loop
function loop() {
    update();
    draw();
    requestAnimationFrame(loop);
}

// Start
loop();
