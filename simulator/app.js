/* =============================================================
 * Mandy Wildlife Tracker — display preview simulator
 *
 * A little desk companion that follows ONE special penguin — Luna,
 * a King Penguin wandering the Southern Ocean — and shows her latest
 * known location on a magical vintage field-guide chart.
 *
 * Renders the exact 648 x 480 UI the Waveshare 5.83" (G) panel shows,
 * using ONLY the four colours the panel supports: black, white,
 * yellow, red. Everything is drawn in solid panel colours (no alpha),
 * because e-paper can't show translucency or grey.
 * ============================================================= */

"use strict";

// --- Panel constants -----------------------------------------
const W = 648;
const H = 480;

const PANEL = {
  black:  { css: "#000000", code: 0 },
  white:  { css: "#ffffff", code: 1 },
  yellow: { css: "#e8c400", code: 2 },
  red:    { css: "#d0121b", code: 3 },
};

// --- The one tracked animal ----------------------------------
// Luna the King Penguin. Coordinates are illustrative for the preview;
// the firmware will fill these from real tracking data later.
const LUNA = {
  name: "LUNA",
  species: "King Penguin",
  locationName: "Southern Ocean",
  lat: -54.8123,
  lng: 158.4231,
  distanceKm: 1284.7,
  lastSignalMin: 34,
  blurb: "wintering among the southern isles",
};

// --- Layout regions (device pixels) --------------------------
const LAYOUT = {
  margin: 12,
  border: 8,      // decorative outer frame inset
  headerH: 58,
  footerH: 98,
};

// -------------------------------------------------------------
const canvas = document.getElementById("epd");
const ctx = canvas.getContext("2d");
ctx.imageSmoothingEnabled = false;

// Stable pseudo-random generator so the scene (stars, waves, coast)
// is the same every render rather than jittering.
function makeRng(seed) {
  let s = seed >>> 0;
  return function () {
    s = (s * 1664525 + 1013904223) >>> 0;
    return s / 4294967296;
  };
}

// =============================================================
// Decorative frame — an antique map border with corner flourishes
// =============================================================

function drawDecorFrame() {
  const b = LAYOUT.border;

  ctx.strokeStyle = PANEL.black.css;
  ctx.lineWidth = 3;
  ctx.strokeRect(b + 1.5, b + 1.5, W - 2 * b - 3, H - 2 * b - 3);

  ctx.lineWidth = 1;
  ctx.strokeRect(b + 6.5, b + 6.5, W - 2 * b - 13, H - 2 * b - 13);

  const corners = [
    [b + 6, b + 6], [W - b - 6, b + 6],
    [b + 6, H - b - 6], [W - b - 6, H - b - 6],
  ];
  ctx.fillStyle = PANEL.red.css;
  for (const [cx, cy] of corners) diamond(cx, cy, 5);
}

function diamond(cx, cy, r) {
  ctx.beginPath();
  ctx.moveTo(cx, cy - r);
  ctx.lineTo(cx + r, cy);
  ctx.lineTo(cx, cy + r);
  ctx.lineTo(cx - r, cy);
  ctx.closePath();
  ctx.fill();
}

function contentBox() {
  const inset = LAYOUT.border + 12;
  return { x: inset, y: inset, w: W - 2 * inset, h: H - 2 * inset };
}

// =============================================================
// Header
// =============================================================

function drawHeader(box) {
  const x = box.x;
  const y = box.y;

  ctx.fillStyle = PANEL.black.css;
  ctx.textBaseline = "alphabetic";
  ctx.textAlign = "left";
  ctx.font = "700 27px Georgia, 'Times New Roman', serif";
  ctx.fillText("Mandy's Penguin", x, y + 26);

  const tw = ctx.measureText("Mandy's Penguin").width;
  drawHeart(x + tw + 14, y + 16, 7);

  ctx.font = "italic 13px Georgia, serif";
  ctx.fillText("following one small wanderer of the Southern Ocean", x + 1, y + 45);

  // Species badge, red with white text.
  const label = LUNA.species.toUpperCase();
  ctx.font = "700 15px Georgia, serif";
  const lw = ctx.measureText(label).width;
  const bw = lw + 22;
  const bx = box.x + box.w - bw;
  ctx.fillStyle = PANEL.red.css;
  fillRoundRect(bx, y + 2, bw, 28, 4);
  ctx.fillStyle = PANEL.white.css;
  ctx.fillText(label, bx + 11, y + 21);

  const ry = y + LAYOUT.headerH - 6;
  ctx.fillStyle = PANEL.black.css;
  ctx.fillRect(x, ry, box.w, 2);
  ctx.fillRect(x, ry + 4, box.w, 1);
}

function drawHeart(cx, cy, s) {
  ctx.fillStyle = PANEL.red.css;
  ctx.beginPath();
  ctx.moveTo(cx, cy + s * 0.9);
  ctx.bezierCurveTo(cx - s * 1.4, cy - s * 0.4, cx - s * 0.5, cy - s * 1.2, cx, cy - s * 0.3);
  ctx.bezierCurveTo(cx + s * 0.5, cy - s * 1.2, cx + s * 1.4, cy - s * 0.4, cx, cy + s * 0.9);
  ctx.closePath();
  ctx.fill();
}

// =============================================================
// The magical polar chart
// =============================================================

function mapRect(box) {
  const top = box.y + LAYOUT.headerH + 4;
  const bottom = box.y + box.h - LAYOUT.footerH;
  return { x: box.x, y: top, w: box.w, h: bottom - top };
}

function drawMapFrame(r) {
  ctx.strokeStyle = PANEL.black.css;
  ctx.lineWidth = 2;
  ctx.strokeRect(r.x + 0.5, r.y + 0.5, r.w - 1, r.h - 1);
}

function drawScene(r) {
  const rng = makeRng(0xC0FFEE);

  ctx.save();
  ctx.beginPath();
  ctx.rect(r.x + 2, r.y + 2, r.w - 4, r.h - 4);
  ctx.clip();
  const inner = { x: r.x + 2, y: r.y + 2, w: r.w - 4, h: r.h - 4 };

  // Sea: yellow wash for the Southern Ocean.
  ctx.fillStyle = PANEL.yellow.css;
  ctx.fillRect(inner.x, inner.y, inner.w, inner.h);

  // A faint nautical graticule.
  drawGraticule(inner);

  // A scatter of little stars/sparkles for a magical night-sea feel.
  drawStars(inner, rng);

  // Gentle wave crests across the open water.
  drawWaves(inner, rng);

  // White ice shelf along the bottom, plus a couple of drifting islands.
  drawIce(inner, rng);

  // Region label, engraved italic, upper-left of the sea.
  ctx.fillStyle = PANEL.black.css;
  ctx.font = "italic 700 15px Georgia, serif";
  ctx.textAlign = "left";
  ctx.fillText("SOUTHERN OCEAN", inner.x + 14, inner.y + 24);

  // Luna's journey and where she is now.
  const route = buildRoute(r, rng);
  const here = route[route.length - 1];
  drawRoute(route);

  // The hero: a lovely King Penguin standing by her marker.
  drawPenguin(here.x - 4, here.y - 6);
  drawMarker(here.x + 20, here.y + 6);

  ctx.restore();

  // Overlays kept inside the frame.
  drawCompass(r.x + r.w - 40, r.y + 42);
  drawScaleBar(r.x + r.w - 132, r.y + r.h - 20);
}

function drawGraticule(inner) {
  ctx.strokeStyle = PANEL.black.css;
  ctx.lineWidth = 1;
  ctx.setLineDash([1, 8]);
  const stepX = inner.w / 6;
  const stepY = inner.h / 4;
  ctx.beginPath();
  for (let i = 1; i < 6; i++) {
    const x = Math.round(inner.x + stepX * i) + 0.5;
    ctx.moveTo(x, inner.y); ctx.lineTo(x, inner.y + inner.h);
  }
  for (let i = 1; i < 4; i++) {
    const y = Math.round(inner.y + stepY * i) + 0.5;
    ctx.moveTo(inner.x, y); ctx.lineTo(inner.x + inner.w, y);
  }
  ctx.stroke();
  ctx.setLineDash([]);
}

// A few larger four-point sparkles over the sea — the "magic".
// Fewer and bigger reads as intentional stars rather than noise.
function drawStars(inner, rng) {
  const n = 12;
  for (let i = 0; i < n; i++) {
    const x = inner.x + 30 + rng() * (inner.w - 60);
    const y = inner.y + 40 + rng() * (inner.h * 0.5);
    const s = rng() < 0.4 ? 6.5 : 4.5;
    sparkle(x, y, s);
  }
}

function sparkle(cx, cy, s) {
  // A white four-point star with a crisp black outline so it reads
  // clearly on the yellow sea. Slim points give it a twinkle.
  ctx.fillStyle = PANEL.white.css;
  ctx.strokeStyle = PANEL.black.css;
  ctx.lineWidth = 1;
  ctx.beginPath();
  ctx.moveTo(cx, cy - s);
  ctx.lineTo(cx + s * 0.22, cy - s * 0.22);
  ctx.lineTo(cx + s, cy);
  ctx.lineTo(cx + s * 0.22, cy + s * 0.22);
  ctx.lineTo(cx, cy + s);
  ctx.lineTo(cx - s * 0.22, cy + s * 0.22);
  ctx.lineTo(cx - s, cy);
  ctx.lineTo(cx - s * 0.22, cy - s * 0.22);
  ctx.closePath();
  ctx.fill();
  ctx.stroke();
}

function drawWaves(inner, rng) {
  ctx.strokeStyle = PANEL.black.css;
  ctx.lineWidth = 1;
  ctx.beginPath();
  let row = 0;
  // Only over the open water (upper ~62%), leaving the ice clear below.
  const yMax = inner.y + inner.h * 0.62;
  for (let y = inner.y + 40; y < yMax; y += 30) {
    const offset = (row % 2) * 26;
    for (let x = inner.x + 22 + offset; x < inner.x + inner.w - 48; x += 52) {
      ctx.moveTo(x, y);
      ctx.quadraticCurveTo(x + 6, y - 3.5, x + 12, y);
      ctx.quadraticCurveTo(x + 18, y + 3.5, x + 24, y);
    }
    row++;
  }
  ctx.stroke();
}

function drawIce(inner, rng) {
  const iceTop = inner.y + inner.h * 0.66;

  // Main ice shelf hugging the bottom, with a soft wavy top edge.
  ctx.fillStyle = PANEL.white.css;
  ctx.strokeStyle = PANEL.black.css;
  ctx.lineWidth = 2;
  ctx.beginPath();
  ctx.moveTo(inner.x, inner.y + inner.h);
  ctx.lineTo(inner.x, iceTop + 14);
  let x = inner.x;
  const step = 46;
  let up = true;
  while (x < inner.x + inner.w) {
    const nx = Math.min(x + step, inner.x + inner.w);
    const cy = iceTop + (up ? -6 : 12) + (makeRng(x)() - 0.5) * 6;
    ctx.quadraticCurveTo((x + nx) / 2, cy, nx, iceTop + 10);
    x = nx; up = !up;
  }
  ctx.lineTo(inner.x + inner.w, inner.y + inner.h);
  ctx.closePath();
  ctx.fill();
  ctx.stroke();

  // A couple of gentle contour lines on the ice for subtle texture,
  // following the shelf rather than random ticks.
  ctx.lineWidth = 1;
  ctx.strokeStyle = PANEL.black.css;
  const bottom = inner.y + inner.h;
  for (let c = 0; c < 2; c++) {
    const yBase = iceTop + 24 + c * 20;
    ctx.beginPath();
    for (let sx = inner.x + 20; sx < inner.x + inner.w - 20; sx += 8) {
      const yy = yBase + Math.sin((sx + c * 40) * 0.03) * 3;
      if (sx === inner.x + 20) ctx.moveTo(sx, yy);
      else ctx.lineTo(sx, yy);
    }
    ctx.stroke();
  }

  // Two little drifting ice floes out on the water.
  for (let k = 0; k < 2; k++) {
    const ix = inner.x + 60 + rng() * (inner.w - 180);
    const iy = inner.y + 60 + rng() * (inner.h * 0.4);
    floe(ix, iy, 24 + rng() * 20, 12 + rng() * 8, makeRng(k * 91 + 5));
  }
}

function floe(cx, cy, rx, ry, rng) {
  ctx.fillStyle = PANEL.white.css;
  ctx.strokeStyle = PANEL.black.css;
  ctx.lineWidth = 2;
  const pts = 12;
  ctx.beginPath();
  for (let i = 0; i <= pts; i++) {
    const a = (i / pts) * Math.PI * 2;
    const wob = 0.82 + rng() * 0.3;
    const x = cx + Math.cos(a) * rx * wob;
    const y = cy + Math.sin(a) * ry * wob;
    if (i === 0) ctx.moveTo(x, y); else ctx.lineTo(x, y);
  }
  ctx.closePath();
  ctx.fill();
  ctx.stroke();
}

function buildRoute(r, rng) {
  // A wandering path that ends on the ice shelf where Luna stands.
  const pts = [];
  const n = 7;
  const startX = r.x + 40;
  const endX = r.x + r.w * 0.44;
  const step = (endX - startX) / (n - 1);
  let x = startX;
  let y = r.y + r.h * 0.34;
  for (let i = 0; i < n; i++) {
    pts.push({ x, y });
    x += step;
    y += (rng() - 0.5) * r.h * 0.14 + r.h * 0.03; // gently descends toward ice
    y = Math.max(r.y + 40, Math.min(r.y + r.h * 0.6, y));
  }
  return pts;
}

function drawRoute(route) {
  ctx.strokeStyle = PANEL.black.css;
  ctx.lineWidth = 2;
  ctx.setLineDash([6, 5]);
  ctx.beginPath();
  ctx.moveTo(route[0].x, route[0].y);
  for (let i = 1; i < route.length; i++) {
    const p = route[i - 1], q = route[i];
    const mx = (p.x + q.x) / 2;
    ctx.quadraticCurveTo(p.x, p.y, mx, (p.y + q.y) / 2);
    ctx.lineTo(q.x, q.y);
  }
  ctx.stroke();
  ctx.setLineDash([]);

  for (let i = 0; i < route.length - 1; i++) {
    ctx.fillStyle = PANEL.white.css;
    ctx.beginPath();
    ctx.arc(route[i].x, route[i].y, 3, 0, Math.PI * 2);
    ctx.fill();
    ctx.strokeStyle = PANEL.black.css;
    ctx.lineWidth = 1.5;
    ctx.stroke();
  }
}

function drawMarker(x, y) {
  // Red "you are here" teardrop pin.
  ctx.fillStyle = PANEL.black.css;
  ctx.beginPath();
  ctx.ellipse(x, y + 2, 4, 1.6, 0, 0, Math.PI * 2);
  ctx.fill();

  ctx.fillStyle = PANEL.red.css;
  ctx.strokeStyle = PANEL.black.css;
  ctx.lineWidth = 1.5;
  ctx.beginPath();
  ctx.arc(x, y - 11, 8, Math.PI, Math.PI * 2);
  ctx.lineTo(x, y + 1);
  ctx.closePath();
  ctx.fill();
  ctx.stroke();
  ctx.fillStyle = PANEL.white.css;
  ctx.beginPath();
  ctx.arc(x, y - 11, 3, 0, Math.PI * 2);
  ctx.fill();
}

// =============================================================
// The hero — a lovely King Penguin, drawn large on the ice.
// Anchor (px,py) is roughly the penguin's feet.
// =============================================================

function drawPenguin(px, py) {
  // A cute, cartoony King Penguin built from simple rounded shapes:
  // a round head on a chubby egg body, a big white face mask and belly,
  // stubby flippers, a little beak and rounded feet. Anchor (px,py) is
  // at the feet. Everything is solid panel colours (no alpha).
  ctx.save();
  ctx.translate(px, py);
  ctx.lineJoin = "round";
  ctx.lineCap = "round";

  ctx.strokeStyle = PANEL.black.css;

  const bodyCx = 0;
  const bodyCy = -26;      // centre of the chubby body
  const bodyRx = 24;
  const bodyRy = 30;
  const headCy = -62;      // centre of the head
  const headR = 19;

  // --- Feet (yellow, tucked under the body) ---
  ctx.fillStyle = PANEL.yellow.css;
  ctx.lineWidth = 1.4;
  foot(-9, 2);
  foot(9, 2);

  // --- Flippers (behind the body so they read as arms) ---
  ctx.fillStyle = PANEL.black.css;
  flipper(-bodyRx + 3, bodyCy - 4, -1);
  flipper(bodyRx - 3, bodyCy - 4, 1);

  // --- Body (black egg) ---
  ctx.fillStyle = PANEL.black.css;
  ctx.beginPath();
  ctx.ellipse(bodyCx, bodyCy, bodyRx, bodyRy, 0, 0, Math.PI * 2);
  ctx.fill();

  // --- White belly (big soft oval on the front) ---
  ctx.fillStyle = PANEL.white.css;
  ctx.beginPath();
  ctx.ellipse(bodyCx, bodyCy + 2, bodyRx - 8, bodyRy - 7, 0, 0, Math.PI * 2);
  ctx.fill();

  // --- Head (black circle) ---
  ctx.fillStyle = PANEL.black.css;
  ctx.beginPath();
  ctx.arc(0, headCy, headR, 0, Math.PI * 2);
  ctx.fill();

  // --- White face mask (so the eyes sit on white — the cute bit) ---
  ctx.fillStyle = PANEL.white.css;
  ctx.beginPath();
  ctx.ellipse(0, headCy + 2, headR - 5, headR - 3, 0, 0, Math.PI * 2);
  ctx.fill();

  // --- King Penguin ear-patches: yellow arcs hugging the head sides ---
  ctx.fillStyle = PANEL.yellow.css;
  ctx.beginPath();
  ctx.moveTo(-headR + 2, headCy - 2);
  ctx.quadraticCurveTo(-headR - 2, headCy + 6, -headR + 6, headCy + 10);
  ctx.quadraticCurveTo(-headR + 3, headCy + 2, -headR + 2, headCy - 2);
  ctx.closePath();
  ctx.fill();
  ctx.beginPath();
  ctx.moveTo(headR - 2, headCy - 2);
  ctx.quadraticCurveTo(headR + 2, headCy + 6, headR - 6, headCy + 10);
  ctx.quadraticCurveTo(headR - 3, headCy + 2, headR - 2, headCy - 2);
  ctx.closePath();
  ctx.fill();

  // --- Eyes (big and friendly, with a white highlight) ---
  ctx.fillStyle = PANEL.black.css;
  ctx.beginPath();
  ctx.arc(-6, headCy + 1, 3.2, 0, Math.PI * 2);
  ctx.arc(6, headCy + 1, 3.2, 0, Math.PI * 2);
  ctx.fill();
  ctx.fillStyle = PANEL.white.css;
  ctx.beginPath();
  ctx.arc(-5, headCy - 0.4, 1.1, 0, Math.PI * 2);
  ctx.arc(7, headCy - 0.4, 1.1, 0, Math.PI * 2);
  ctx.fill();

  // --- Beak (little yellow triangle, with a red lower half) ---
  ctx.fillStyle = PANEL.yellow.css;
  ctx.beginPath();
  ctx.moveTo(-4, headCy + 7);
  ctx.lineTo(4, headCy + 7);
  ctx.lineTo(0, headCy + 13);
  ctx.closePath();
  ctx.fill();
  ctx.strokeStyle = PANEL.black.css;
  ctx.lineWidth = 1;
  ctx.stroke();
  ctx.fillStyle = PANEL.red.css;
  ctx.beginPath();
  ctx.moveTo(-2.4, headCy + 10);
  ctx.lineTo(2.4, headCy + 10);
  ctx.lineTo(0, headCy + 13);
  ctx.closePath();
  ctx.fill();

  // --- Rosy cheeks (small yellow dots) for a touch of charm ---
  ctx.fillStyle = PANEL.yellow.css;
  ctx.beginPath();
  ctx.arc(-11, headCy + 5, 2, 0, Math.PI * 2);
  ctx.arc(11, headCy + 5, 2, 0, Math.PI * 2);
  ctx.fill();

  ctx.restore();
}

function foot(dx, dy) {
  // A little rounded yellow foot.
  ctx.beginPath();
  ctx.ellipse(dx, dy, 7, 3.4, 0, 0, Math.PI * 2);
  ctx.fill();
  ctx.stroke();
}

function flipper(x, y, dir) {
  // A stubby black flipper sweeping down and out from the shoulder.
  ctx.beginPath();
  ctx.moveTo(x, y);
  ctx.quadraticCurveTo(x + dir * 12, y + 10, x + dir * 6, y + 30);
  ctx.quadraticCurveTo(x + dir * 1, y + 22, x - dir * 1, y + 4);
  ctx.closePath();
  ctx.fill();
}

// =============================================================
// Compass rose (8-point) + scale bar
// =============================================================

function drawCompass(cx, cy) {
  const R = 15;
  ctx.fillStyle = PANEL.white.css;
  ctx.beginPath(); ctx.arc(cx, cy, R + 3, 0, Math.PI * 2); ctx.fill();
  ctx.strokeStyle = PANEL.black.css;
  ctx.lineWidth = 1.5;
  ctx.beginPath(); ctx.arc(cx, cy, R + 3, 0, Math.PI * 2); ctx.stroke();

  ctx.fillStyle = PANEL.black.css;
  for (let k = 0; k < 4; k++) {
    const a = Math.PI / 4 + k * (Math.PI / 2);
    compassPoint(cx, cy, a, R * 0.7, 2.5);
  }
  for (let k = 0; k < 4; k++) {
    const a = k * (Math.PI / 2);
    ctx.fillStyle = (k === 3) ? PANEL.red.css : PANEL.black.css; // north (-y) red
    compassPoint(cx, cy, a, R, 3.5);
  }
  ctx.fillStyle = PANEL.black.css;
  ctx.font = "700 9px Georgia, serif";
  ctx.textAlign = "center";
  ctx.fillText("N", cx, cy - R - 5);
  ctx.beginPath(); ctx.arc(cx, cy, 1.6, 0, Math.PI * 2); ctx.fill();
}

function compassPoint(cx, cy, a, len, half) {
  const tipX = cx + Math.cos(a) * len;
  const tipY = cy + Math.sin(a) * len;
  const bx = cx + Math.cos(a + Math.PI / 2) * half;
  const by = cy + Math.sin(a + Math.PI / 2) * half;
  const dx = cx + Math.cos(a - Math.PI / 2) * half;
  const dy = cy + Math.sin(a - Math.PI / 2) * half;
  ctx.beginPath();
  ctx.moveTo(tipX, tipY);
  ctx.lineTo(bx, by);
  ctx.lineTo(dx, dy);
  ctx.closePath();
  ctx.fill();
}

function drawScaleBar(x, y) {
  ctx.fillStyle = PANEL.white.css;
  ctx.fillRect(x - 3, y - 10, 108, 20);
  ctx.fillStyle = PANEL.black.css;
  const seg = 25;
  for (let i = 0; i < 4; i++) if (i % 2 === 0) ctx.fillRect(x + i * seg, y, seg, 3);
  ctx.strokeStyle = PANEL.black.css;
  ctx.lineWidth = 1;
  ctx.strokeRect(x + 0.5, y + 0.5, 100, 3);
  for (let i = 0; i <= 100; i += 25) ctx.fillRect(x + i, y - 3, 1, 9);
  ctx.font = "9px Georgia, serif";
  ctx.textAlign = "left";
  ctx.fillText("0", x - 1, y - 6);
  ctx.textAlign = "right";
  ctx.fillText("500 km", x + 100, y - 6);
}

// =============================================================
// Footer
// =============================================================

function drawFooter(box) {
  const x = box.x;
  const y = box.y + box.h - LAYOUT.footerH + 6;

  ctx.fillStyle = PANEL.black.css;
  ctx.textAlign = "left";
  ctx.font = "700 25px Georgia, serif";
  ctx.fillText(cap(LUNA.name), x, y + 20);

  ctx.font = "italic 15px Georgia, serif";
  ctx.textAlign = "right";
  ctx.fillText(LUNA.species, box.x + box.w, y + 20);

  ctx.textAlign = "left";
  ctx.font = "13px Georgia, serif";
  ctx.fillText("Latest known location \u2014 " + LUNA.blurb, x, y + 40);

  ctx.textAlign = "right";
  ctx.font = "12px Georgia, serif";
  ctx.fillText(formatCoords(LUNA.lat, LUNA.lng), box.x + box.w, y + 40);

  ctx.textAlign = "left";
  ctx.font = "700 16px Georgia, serif";
  ctx.fillText(LUNA.locationName, x, y + 60);

  ctx.fillStyle = PANEL.black.css;
  ctx.fillRect(x, y + 68, box.w, 1);

  ctx.font = "13px Georgia, serif";
  ctx.textAlign = "left";
  const stale = LUNA.lastSignalMin > 60;
  ctx.fillStyle = stale ? PANEL.red.css : PANEL.black.css;
  ctx.fillText("Last signal: " + formatAge(LUNA.lastSignalMin), x, y + 85);

  // Centre signature.
  ctx.fillStyle = PANEL.black.css;
  ctx.textAlign = "center";
  drawHeart(box.x + box.w / 2 - 46, y + 80, 5);
  ctx.font = "italic 12px Georgia, serif";
  ctx.fillText("Made for Mandy", box.x + box.w / 2 + 4, y + 85);

  ctx.textAlign = "right";
  ctx.font = "13px Georgia, serif";
  ctx.fillText(formatKm(LUNA.distanceKm) + " travelled", box.x + box.w, y + 85);
}

// --- Ashore / moulting variant -------------------------------
// Luna is at the colony (not at sea): show her standing at home on the
// ice, and a footer that explains the stillness with a day counter.

const LUNA_MOULT = {
  phaseLabel: "Moulting ashore",
  phaseExplain: "Ashore for her yearly moult, fasting as new feathers grow.",
  phaseDay: 6,
  phaseTotalDays: 32,
  distanceKm: 3120,   // total this year so far
  journeyDay: 256,
  journeyTotalDays: 365,
};

function drawColonyScene(r) {
  ctx.save();
  ctx.beginPath();
  ctx.rect(r.x + 2, r.y + 2, r.w - 4, r.h - 4);
  ctx.clip();
  const inner = { x: r.x + 2, y: r.y + 2, w: r.w - 4, h: r.h - 4 };

  // Yellow sea backdrop with a big white ice/colony shore filling most
  // of the frame — she's home, so the colony is the focus, not the sea.
  ctx.fillStyle = PANEL.yellow.css;
  ctx.fillRect(inner.x, inner.y, inner.w, inner.h);
  drawStars(inner, makeRng(0xC0FFEE));

  // Ice/land shore across the lower two-thirds.
  const shoreTop = inner.y + inner.h * 0.42;
  ctx.fillStyle = PANEL.white.css;
  ctx.strokeStyle = PANEL.black.css;
  ctx.lineWidth = 2;
  let px = inner.x, py = shoreTop, up = true;
  ctx.beginPath();
  ctx.moveTo(inner.x, inner.y + inner.h);
  ctx.lineTo(inner.x, shoreTop);
  for (let x = inner.x; x <= inner.x + inner.w; x += 44) {
    const yy = shoreTop + (up ? -7 : 9);
    ctx.lineTo(x, yy); up = !up;
  }
  ctx.lineTo(inner.x + inner.w, inner.y + inner.h);
  ctx.closePath();
  ctx.fill();
  ctx.stroke();

  // Region label.
  ctx.fillStyle = PANEL.black.css;
  ctx.font = "italic 700 15px Georgia, serif";
  ctx.textAlign = "left";
  ctx.fillText("CROZET ISLANDS \u2014 THE COLONY", inner.x + 14, inner.y + 22);

  // A few other little penguins to suggest the colony (small, behind).
  drawMiniPenguin(inner.x + inner.w * 0.30, shoreTop + 40);
  drawMiniPenguin(inner.x + inner.w * 0.68, shoreTop + 52);
  drawMiniPenguin(inner.x + inner.w * 0.80, shoreTop + 34);

  // Luna, front and centre on the shore.
  const lunaX = inner.x + inner.w * 0.5;
  const lunaY = shoreTop + 78;
  drawPenguin(lunaX, lunaY);
  drawMarker(lunaX + 26, lunaY - 30);

  ctx.restore();

  drawCompass(r.x + r.w - 40, r.y + 42);
}

function drawMiniPenguin(cx, cy) {
  ctx.save();
  ctx.translate(cx, cy);
  ctx.scale(0.42, 0.42);
  drawPenguin(0, 0);
  ctx.restore();
}

function drawMoultFooter(box) {
  const x = box.x;
  const y = box.y + box.h - LAYOUT.footerH + 6;

  ctx.fillStyle = PANEL.black.css;
  ctx.textAlign = "left";
  ctx.font = "700 25px Georgia, serif";
  ctx.fillText(cap(LUNA.name), x, y + 20);

  ctx.font = "italic 15px Georgia, serif";
  ctx.textAlign = "right";
  ctx.fillText(LUNA.species, box.x + box.w, y + 20);

  // Phase label (left) + coordinates (right).
  ctx.textAlign = "left";
  ctx.font = "700 15px Georgia, serif";
  ctx.fillText(LUNA_MOULT.phaseLabel, x, y + 40);

  ctx.textAlign = "right";
  ctx.font = "12px Georgia, serif";
  ctx.fillText(formatCoords(JOURNEY_COLONY_LAT, JOURNEY_COLONY_LNG), box.x + box.w, y + 40);

  // Explanation line (small) — tells you WHY she isn't moving.
  ctx.textAlign = "left";
  ctx.font = "13px Georgia, serif";
  ctx.fillText(LUNA_MOULT.phaseExplain, x, y + 58);

  ctx.fillStyle = PANEL.black.css;
  ctx.fillRect(x, y + 68, box.w, 1);

  // Day-of-phase counter (left) + km-this-year (right).
  ctx.font = "700 14px Georgia, serif";
  ctx.textAlign = "left";
  ctx.fillText(
    "Day " + LUNA_MOULT.phaseDay + " of " + LUNA_MOULT.phaseTotalDays + " ashore",
    x, y + 85
  );

  ctx.fillStyle = PANEL.black.css;
  ctx.textAlign = "center";
  drawHeart(box.x + box.w / 2 - 46, y + 80, 5);
  ctx.font = "italic 12px Georgia, serif";
  ctx.fillText("Made for Mandy", box.x + box.w / 2 + 4, y + 85);

  ctx.textAlign = "right";
  ctx.font = "13px Georgia, serif";
  ctx.fillText(formatKm(LUNA_MOULT.distanceKm) + " this year", box.x + box.w, y + 85);
}

// Colony coords for the moult preview (match PenguinJourney.h).
const JOURNEY_COLONY_LAT = -46.43;
const JOURNEY_COLONY_LNG = 51.86;

// =============================================================
// Alternate screens
// =============================================================

function drawBootScreen() {
  fillBackground();
  drawDecorFrame();
  const cx = W / 2;

  // A little penguin above the title for warmth.
  ctx.save();
  ctx.translate(cx, H / 2 - 46);
  ctx.scale(0.9, 0.9);
  drawPenguin(0, 34);
  ctx.restore();

  ctx.fillStyle = PANEL.black.css;
  ctx.textAlign = "center";
  ctx.font = "700 34px Georgia, serif";
  ctx.fillText("Mandy's Penguin", cx, H / 2 + 58);
  drawHeart(cx, H / 2 + 78, 10);
  ctx.fillStyle = PANEL.red.css;
  ctx.font = "700 15px Georgia, serif";
  ctx.fillText("Made for Mandy", cx, H / 2 + 104);
}

function drawWiFiScreen() {
  fillBackground();
  drawDecorFrame();
  const cx = W / 2, cy = H / 2 + 40;
  ctx.fillStyle = PANEL.black.css;
  ctx.textAlign = "center";
  ctx.font = "700 26px Georgia, serif";
  ctx.fillText("Finding Luna\u2026", cx, H / 2 - 20);
  for (let i = 1; i <= 3; i++) {
    ctx.strokeStyle = i === 3 ? PANEL.red.css : PANEL.black.css;
    ctx.lineWidth = 3;
    ctx.beginPath();
    ctx.arc(cx, cy, i * 15, Math.PI * 1.25, Math.PI * 1.75);
    ctx.stroke();
  }
  ctx.fillStyle = PANEL.black.css;
  ctx.beginPath(); ctx.arc(cx, cy, 3, 0, Math.PI * 2); ctx.fill();
}

function drawErrorScreen() {
  fillBackground();
  drawDecorFrame();
  const cx = W / 2;
  ctx.fillStyle = PANEL.red.css;
  fillRoundRect(cx - 140, H / 2 - 74, 280, 44, 6);
  ctx.fillStyle = PANEL.white.css;
  ctx.textAlign = "center";
  ctx.font = "700 22px Georgia, serif";
  ctx.fillText("Signal lost", cx, H / 2 - 44);
  ctx.fillStyle = PANEL.black.css;
  ctx.font = "15px Georgia, serif";
  ctx.fillText("Couldn't reach Luna just now.", cx, H / 2);
  ctx.fillText("Showing her last known location.", cx, H / 2 + 24);
}

// =============================================================
// Shared helpers
// =============================================================

function fillBackground() {
  ctx.fillStyle = PANEL.white.css;
  ctx.fillRect(0, 0, W, H);
}

function fillRoundRect(x, y, w, h, r) {
  ctx.beginPath();
  ctx.moveTo(x + r, y);
  ctx.arcTo(x + w, y, x + w, y + h, r);
  ctx.arcTo(x + w, y + h, x, y + h, r);
  ctx.arcTo(x, y + h, x, y, r);
  ctx.arcTo(x, y, x + w, y, r);
  ctx.closePath();
  ctx.fill();
}

function cap(name) {
  return name.charAt(0) + name.slice(1).toLowerCase();
}

function formatCoords(lat, lng) {
  const ns = lat >= 0 ? "N" : "S";
  const ew = lng >= 0 ? "E" : "W";
  return (
    Math.abs(lat).toFixed(4) + "\u00B0 " + ns + "   " +
    Math.abs(lng).toFixed(4) + "\u00B0 " + ew
  );
}

function formatKm(km) {
  return km.toLocaleString("en-GB", { maximumFractionDigits: 0 }) + " km";
}

function formatAge(min) {
  if (min < 60) return min + " min ago";
  const h = Math.floor(min / 60);
  const m = min % 60;
  return m ? `${h} h ${m} min ago` : `${h} h ago`;
}

// =============================================================
// Quantisation to the 4 panel colours (luminance-aware, no alpha)
// =============================================================

const PALETTE_RGB = [
  [0, 0, 0],       // black
  [255, 255, 255], // white
  [232, 196, 0],   // yellow
  [208, 18, 27],   // red
];

function nearestPaletteIndex(r, g, b) {
  const lum = 0.299 * r + 0.587 * g + 0.114 * b;
  if (lum < 110) return 0;                                   // dark -> black
  if (r > 150 && g < 120 && b < 120 && (r - g) > 60) return 3; // red-ish -> red
  if (b < 150 && (r + g) / 2 - b > 60) return 2;            // warm/bright -> yellow
  return 1;                                                 // else white
}

function quantiseToPanel() {
  const img = ctx.getImageData(0, 0, W, H);
  const d = img.data;
  for (let i = 0; i < d.length; i += 4) {
    const idx = nearestPaletteIndex(d[i], d[i + 1], d[i + 2]);
    const [r, g, b] = PALETTE_RGB[idx];
    d[i] = r; d[i + 1] = g; d[i + 2] = b; d[i + 3] = 255;
  }
  ctx.putImageData(img, 0, 0);
}

function drawGridOverlay() {
  ctx.strokeStyle = "rgba(0,0,0,0.12)";
  ctx.lineWidth = 1;
  for (let x = 0; x <= W; x += 24) {
    ctx.beginPath(); ctx.moveTo(x + 0.5, 0); ctx.lineTo(x + 0.5, H); ctx.stroke();
  }
  for (let y = 0; y <= H; y += 24) {
    ctx.beginPath(); ctx.moveTo(0, y + 0.5); ctx.lineTo(W, y + 0.5); ctx.stroke();
  }
}

// =============================================================
// Render orchestration
// =============================================================

function render() {
  const screen = document.getElementById("screen").value;

  if (screen === "boot") {
    drawBootScreen();
  } else if (screen === "wifi") {
    drawWiFiScreen();
  } else if (screen === "error") {
    drawErrorScreen();
  } else if (screen === "moult") {
    fillBackground();
    drawDecorFrame();
    const box = contentBox();
    drawHeader(box);
    const r = mapRect(box);
    drawMapFrame(r);
    drawColonyScene(r);
    drawMoultFooter(box);
  } else {
    fillBackground();
    drawDecorFrame();
    const box = contentBox();
    drawHeader(box);
    const r = mapRect(box);
    drawMapFrame(r);
    drawScene(r);
    drawFooter(box);
  }

  quantiseToPanel();

  if (document.getElementById("grid").checked) drawGridOverlay();
}

// =============================================================
// UI wiring
// =============================================================

const CAPTURE = new URLSearchParams(location.search).get("capture") === "1";

function applyScale() {
  // In capture mode, force a big fixed zoom so the panel fills the
  // screenshot crisply regardless of the slider.
  if (CAPTURE) {
    const scale = 1.7;
    canvas.style.width = W * scale + "px";
    canvas.style.height = H * scale + "px";
    return;
  }
  const truesize = document.getElementById("truesize").checked;
  const zoom = parseFloat(document.getElementById("zoom").value);
  const scale = truesize ? 1 : zoom;
  canvas.style.width = W * scale + "px";
  canvas.style.height = H * scale + "px";
  document.getElementById("zoom").disabled = truesize;
}

// URL params:
//   ?screen=animal|moult|boot|wifi|error   preselect a screen
//   ?capture=1                             hide UI chrome for clean shots
function applyUrlOverrides() {
  const s = new URLSearchParams(location.search).get("screen");
  if (s) {
    const sel = document.getElementById("screen");
    if ([...sel.options].some((o) => o.value === s)) sel.value = s;
  }
  if (CAPTURE) document.body.classList.add("capture");
}

function wire() {
  applyUrlOverrides();
  ["screen", "grid"].forEach((id) =>
    document.getElementById(id).addEventListener("change", render)
  );
  document.getElementById("truesize").addEventListener("change", applyScale);
  document.getElementById("zoom").addEventListener("input", applyScale);

  document.getElementById("download").addEventListener("click", () => {
    const a = document.createElement("a");
    a.download = "mandy-penguin-648x480.png";
    a.href = canvas.toDataURL("image/png");
    a.click();
  });

  applyScale();
  render();
}

wire();
