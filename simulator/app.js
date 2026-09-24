/* =============================================================
 * Mandy Wildlife Tracker — display preview simulator
 *
 * Renders the exact 648 x 480 UI the Waveshare 5.83" (G) panel
 * will show, using ONLY the four colours the panel supports:
 *   black, white, yellow, red.
 *
 * Designed as a vintage naturalist field-guide plate — made as
 * a gift, so the styling leans warm and hand-drawn rather than
 * technical. The drawing is organised into the same regions the
 * firmware DisplayManager uses (header / map / info / footer).
 * ============================================================= */

"use strict";

// --- Panel constants -----------------------------------------
const W = 648;
const H = 480;

// The only four colours the physical panel can show.
// Codes match the firmware buffer packing (2 bits per pixel).
const PANEL = {
  black:  { css: "#000000", code: 0 },
  white:  { css: "#ffffff", code: 1 },
  yellow: { css: "#e8c400", code: 2 },
  red:    { css: "#d0121b", code: 3 },
};

// --- Animal presets ------------------------------------------
// Mandy's favourites: penguins, owls, sloths, pangolins, tigers.
// Coordinates are illustrative; the firmware will get real ones.
const PRESETS = {
  luna: {
    name: "LUNA",
    species: "King Penguin",
    locationName: "Southern Ocean",
    lat: -54.8123, lng: 158.4231,
    distanceKm: 1284.7,
    lastSignalMin: 34,
    icon: "penguin",
    biome: "ocean",
    blurb: "wintering among the southern isles",
  },
  ollie: {
    name: "OLLIE",
    species: "Snowy Owl",
    locationName: "Arctic Tundra",
    lat: 69.3412, lng: 88.2010,
    distanceKm: 642.3,
    lastSignalMin: 12,
    icon: "owl",
    biome: "ice",
    blurb: "drifting over the frozen north",
  },
  sid: {
    name: "SID",
    species: "Brown-throated Sloth",
    locationName: "Amazon Basin",
    lat: -3.4653, lng: -62.2159,
    distanceKm: 3.8,
    lastSignalMin: 128,
    icon: "sloth",
    biome: "forest",
    blurb: "in no particular hurry at all",
  },
  pip: {
    name: "PIP",
    species: "Ground Pangolin",
    locationName: "Kalahari",
    lat: -22.9576, lng: 21.8569,
    distanceKm: 57.1,
    lastSignalMin: 76,
    icon: "pangolin",
    biome: "desert",
    blurb: "snuffling across the red sands",
  },
  raja: {
    name: "RAJA",
    species: "Bengal Tiger",
    locationName: "Sundarbans",
    lat: 21.9497, lng: 89.1833,
    distanceKm: 214.6,
    lastSignalMin: 47,
    icon: "tiger",
    biome: "forest",
    blurb: "prowling the mangrove maze",
  },
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

// A deterministic little PRNG so the "hand drawn" coastlines and
// route are stable between renders (not jittering every frame).
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

  // Heavy outer rule.
  ctx.strokeStyle = PANEL.black.css;
  ctx.lineWidth = 3;
  ctx.strokeRect(b + 1.5, b + 1.5, W - 2 * b - 3, H - 2 * b - 3);

  // Fine inner rule, offset for the classic "engraved plate" look.
  ctx.lineWidth = 1;
  ctx.strokeRect(b + 6.5, b + 6.5, W - 2 * b - 13, H - 2 * b - 13);

  // Corner flourishes — little diamond + dot in each corner.
  const corners = [
    [b + 6, b + 6], [W - b - 6, b + 6],
    [b + 6, H - b - 6], [W - b - 6, H - b - 6],
  ];
  ctx.fillStyle = PANEL.red.css;
  for (const [cx, cy] of corners) {
    diamond(cx, cy, 5);
  }
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

function drawHeader(animal, box) {
  const x = box.x;
  const y = box.y;

  // Title.
  ctx.fillStyle = PANEL.black.css;
  ctx.textBaseline = "alphabetic";
  ctx.textAlign = "left";
  ctx.font = "700 27px Georgia, 'Times New Roman', serif";
  ctx.fillText("Mandy's Wildlife", x, y + 26);

  // Small heart after the title, in red.
  const tw = ctx.measureText("Mandy's Wildlife").width;
  drawHeart(x + tw + 14, y + 16, 7);

  // Italic subtitle, given its own line with breathing room.
  ctx.font = "italic 13px Georgia, serif";
  ctx.fillStyle = PANEL.black.css;
  ctx.fillText("a field guide to a wandering friend", x + 1, y + 45);

  // Species badge on the right, red with white text.
  const label = animal.species.toUpperCase();
  ctx.font = "700 15px Georgia, serif";
  const lw = ctx.measureText(label).width;
  const bw = lw + 22;
  const bx = box.x + box.w - bw;
  ctx.fillStyle = PANEL.red.css;
  fillRoundRect(bx, y + 2, bw, 28, 4);
  ctx.fillStyle = PANEL.white.css;
  ctx.fillText(label, bx + 11, y + 21);

  // Double rule under the header.
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
// Map
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

function drawMapInterior(r, animal) {
  const rng = makeRng(hashString(animal.name + animal.biome));

  ctx.save();
  ctx.beginPath();
  ctx.rect(r.x + 2, r.y + 2, r.w - 4, r.h - 4);
  ctx.clip();

  const inner = { x: r.x + 2, y: r.y + 2, w: r.w - 4, h: r.h - 4 };

  // Base wash: yellow for water/ice biomes, white for land biomes
  // (so land maps read light with a yellow sea border feel).
  const waterBiome = animal.biome === "ocean" || animal.biome === "ice";
  ctx.fillStyle = waterBiome ? PANEL.yellow.css : PANEL.white.css;
  ctx.fillRect(inner.x, inner.y, inner.w, inner.h);

  // Faint graticule (lat/long grid) so it reads like a real chart.
  drawGraticule(inner);

  // Geography.
  if (waterBiome) {
    drawIslands(inner, rng, animal.biome);
    drawOceanTexture(inner, rng, animal.biome);
  } else {
    drawContinent(inner, rng);
    drawLandTexture(inner, rng, animal.biome);
  }

  // Region label — engraved italic, lower-left.
  ctx.fillStyle = PANEL.black.css;
  ctx.font = "italic 700 15px Georgia, serif";
  ctx.textAlign = "left";
  ctx.fillText(animal.locationName, inner.x + 12, inner.y + inner.h - 14);

  // Route history + current position.
  const route = buildRoute(r, rng);
  drawRoute(route);
  drawAnimalIcon(route[route.length - 1], animal.icon);
  drawMarker(route[route.length - 1]);

  ctx.restore();

  // Overlays that sit above the clip (kept inside the frame).
  drawCompass(r.x + r.w - 38, r.y + 42);
  drawScaleBar(r.x + r.w - 132, r.y + r.h - 20);
}

function drawGraticule(inner) {
  // Fine dashed black grid, solid colour (no alpha) so it quantises
  // cleanly to black. Kept thin and widely spaced so it whispers.
  ctx.strokeStyle = PANEL.black.css;
  ctx.lineWidth = 1;
  ctx.setLineDash([1, 7]);
  const stepX = inner.w / 6;
  const stepY = inner.h / 4;
  ctx.beginPath();
  for (let i = 1; i < 6; i++) {
    const x = Math.round(inner.x + stepX * i) + 0.5;
    ctx.moveTo(x, inner.y);
    ctx.lineTo(x, inner.y + inner.h);
  }
  for (let i = 1; i < 4; i++) {
    const y = Math.round(inner.y + stepY * i) + 0.5;
    ctx.moveTo(inner.x, y);
    ctx.lineTo(inner.x + inner.w, y);
  }
  ctx.stroke();
  ctx.setLineDash([]);
}

function drawContinent(inner, rng) {
  // A large landmass; on land biomes the sea is a yellow rim.
  const cx = inner.x + inner.w * (0.48 + (rng() - 0.5) * 0.1);
  const cy = inner.y + inner.h * 0.54;
  // Yellow sea backdrop already implied by rim; draw a yellow halo
  // then white land with a dark coast so land feels raised.
  ctx.fillStyle = PANEL.yellow.css;
  blob(cx, cy, inner.w * 0.42, inner.h * 0.42, rng, false);
  ctx.fillStyle = PANEL.white.css;
  ctx.strokeStyle = PANEL.black.css;
  ctx.lineWidth = 2;
  blob(cx, cy, inner.w * 0.36, inner.h * 0.34, makeRng(1234), true);
}

function drawIslands(inner, rng, biome) {
  ctx.fillStyle = PANEL.white.css;
  ctx.strokeStyle = PANEL.black.css;
  ctx.lineWidth = 2;
  const count = biome === "ice" ? 4 : 3;
  for (let k = 0; k < count; k++) {
    const ix = inner.x + 44 + rng() * (inner.w - 130);
    const iy = inner.y + 40 + rng() * (inner.h - 110);
    const rx = 22 + rng() * 34;
    const ry = 15 + rng() * 22;
    // White island with a solid black coastline (no soft shadow —
    // e-paper can't show grey, and alpha-black over yellow would
    // quantise to red noise).
    ctx.fillStyle = PANEL.white.css;
    ctx.strokeStyle = PANEL.black.css;
    ctx.lineWidth = 2;
    blob(ix, iy, rx, ry, makeRng(k * 7 + 3), true);
  }
}

// A closed wobbly blob — used for land / islands.
function blob(cx, cy, rx, ry, rng, stroke) {
  const pts = 16;
  ctx.beginPath();
  for (let i = 0; i <= pts; i++) {
    const a = (i / pts) * Math.PI * 2;
    const wob = 0.84 + rng() * 0.28;
    const x = cx + Math.cos(a) * rx * wob;
    const y = cy + Math.sin(a) * ry * wob;
    if (i === 0) ctx.moveTo(x, y);
    else ctx.lineTo(x, y);
  }
  ctx.closePath();
  ctx.fill();
  if (stroke) ctx.stroke();
}

function drawOceanTexture(inner, rng, biome) {
  // Solid black wave crests only — NO alpha. On a 4-colour panel,
  // semi-transparent black over yellow blends to a brown that
  // quantises to RED, which looks like noise. Everything must be a
  // solid panel colour. We keep the waves short and spaced so the
  // sea reads gently rather than busily.
  ctx.strokeStyle = PANEL.black.css;
  ctx.lineWidth = 1;
  ctx.beginPath();
  let row = 0;
  for (let y = inner.y + 24; y < inner.y + inner.h - 28; y += 26) {
    const offset = (row % 2) * 22; // brick-offset the rows
    for (let x = inner.x + 16 + offset; x < inner.x + inner.w - 44; x += 44) {
      ctx.moveTo(x, y);
      ctx.quadraticCurveTo(x + 6, y - 3.5, x + 12, y);
      ctx.quadraticCurveTo(x + 18, y + 3.5, x + 24, y);
    }
    row++;
  }
  ctx.stroke();
}

function drawLandTexture(inner, rng, biome) {
  // Solid black stipple (no alpha). Sparse so it suggests terrain
  // without turning into a grey wash the panel can't show.
  ctx.fillStyle = PANEL.black.css;
  const n = biome === "desert" ? 90 : 70;
  for (let i = 0; i < n; i++) {
    const x = inner.x + 30 + rng() * (inner.w - 90);
    const y = inner.y + 30 + rng() * (inner.h - 70);
    ctx.fillRect(x, y, 1, 1);
  }
}

function buildRoute(r, rng) {
  // A meandering path across the map ending near centre-right,
  // stopping short of the compass so nothing collides.
  const pts = [];
  const n = 7;
  const startX = r.x + 44;
  const endX = r.x + r.w - 104;
  const step = (endX - startX) / (n - 1);
  let x = startX;
  let y = r.y + r.h * (0.46 + rng() * 0.16);
  for (let i = 0; i < n; i++) {
    pts.push({ x, y });
    x += step;
    y += (rng() - 0.5) * r.h * 0.22;
    // Stay clear of the compass band (top-right) and the label row.
    y = Math.max(r.y + 78, Math.min(r.y + r.h - 48, y));
  }
  return pts;
}

function drawRoute(route) {
  // Dashed black route line.
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

  // Hollow dots for past fixes.
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

function drawMarker(p) {
  // Current position: red teardrop pin. A small solid black base
  // dot grounds it (no alpha shadows — the panel can't show grey).
  ctx.fillStyle = PANEL.black.css;
  ctx.beginPath();
  ctx.ellipse(p.x, p.y + 2, 4, 1.6, 0, 0, Math.PI * 2);
  ctx.fill();

  ctx.fillStyle = PANEL.red.css;
  ctx.strokeStyle = PANEL.black.css;
  ctx.lineWidth = 1.5;
  ctx.beginPath();
  ctx.arc(p.x, p.y - 11, 8, Math.PI, Math.PI * 2);
  ctx.lineTo(p.x, p.y + 1);
  ctx.closePath();
  ctx.fill();
  ctx.stroke();
  ctx.fillStyle = PANEL.white.css;
  ctx.beginPath();
  ctx.arc(p.x, p.y - 11, 3, 0, Math.PI * 2);
  ctx.fill();
}

// =============================================================
// Animal illustrations — charming black-line glyphs with accents
// =============================================================

function drawAnimalIcon(p, kind) {
  // The animal sits in a generous white medallion floating just
  // above and left of the current-position pin, joined by a little
  // leader line so it clearly belongs to the marker.
  const R = 27;
  const cx = p.x - 42;
  const cy = p.y - 48;

  ctx.save();

  // Leader line from medallion toward the pin.
  ctx.strokeStyle = PANEL.black.css;
  ctx.lineWidth = 1.5;
  ctx.setLineDash([3, 3]);
  ctx.beginPath();
  ctx.moveTo(cx + R * 0.7, cy + R * 0.7);
  ctx.lineTo(p.x - 6, p.y - 14);
  ctx.stroke();
  ctx.setLineDash([]);

  // Medallion: white fill with a double ring (no alpha shadow —
  // e-paper is solid-colour only).
  ctx.fillStyle = PANEL.white.css;
  ctx.beginPath(); ctx.arc(cx, cy, R, 0, Math.PI * 2); ctx.fill();
  ctx.strokeStyle = PANEL.black.css;
  ctx.lineWidth = 2;
  ctx.beginPath(); ctx.arc(cx, cy, R, 0, Math.PI * 2); ctx.stroke();
  ctx.lineWidth = 1;
  ctx.beginPath(); ctx.arc(cx, cy, R - 3, 0, Math.PI * 2); ctx.stroke();

  // Clip the illustration to the medallion so nothing spills out.
  ctx.save();
  ctx.beginPath(); ctx.arc(cx, cy, R - 4, 0, Math.PI * 2); ctx.clip();

  // Draw the glyph centred in a ~40px design box.
  ctx.translate(cx, cy + 2);
  const scale = 1.6;
  ctx.scale(scale, scale);
  ctx.lineJoin = "round";
  ctx.lineCap = "round";

  switch (kind) {
    case "penguin": glyphPenguin(); break;
    case "owl":     glyphOwl(); break;
    case "sloth":   glyphSloth(); break;
    case "pangolin":glyphPangolin(); break;
    case "tiger":   glyphTiger(); break;
    default:        glyphPenguin();
  }
  ctx.restore();
  ctx.restore();
}

// All glyphs are drawn centred on the origin (0,0) in an
// approximately 26 x 26 design box, then scaled/placed by the caller.

function glyphPenguin() {
  ctx.lineWidth = 1.4;
  // body (black)
  ctx.fillStyle = PANEL.black.css;
  ctx.strokeStyle = PANEL.black.css;
  ctx.beginPath();
  ctx.ellipse(0, 1, 7, 11, 0, 0, Math.PI * 2);
  ctx.fill();
  // white belly
  ctx.fillStyle = PANEL.white.css;
  ctx.beginPath();
  ctx.ellipse(0, 3, 4, 8, 0, 0, Math.PI * 2);
  ctx.fill();
  // white face patch
  ctx.beginPath();
  ctx.arc(0, -6, 4.2, 0, Math.PI * 2);
  ctx.fill();
  // black head crown over the patch
  ctx.fillStyle = PANEL.black.css;
  ctx.beginPath();
  ctx.arc(0, -7.5, 4.2, Math.PI, Math.PI * 2);
  ctx.fill();
  // eyes
  ctx.fillStyle = PANEL.black.css;
  ctx.beginPath();
  ctx.arc(-1.7, -6, 0.9, 0, Math.PI * 2);
  ctx.arc(1.7, -6, 0.9, 0, Math.PI * 2);
  ctx.fill();
  // yellow beak + ear flashes
  ctx.fillStyle = PANEL.yellow.css;
  ctx.beginPath();
  ctx.moveTo(0, -4); ctx.lineTo(3.5, -3); ctx.lineTo(0, -2);
  ctx.closePath(); ctx.fill();
  ctx.beginPath(); ctx.arc(-4, -6, 1.4, 0, Math.PI * 2);
  ctx.arc(4, -6, 1.4, 0, Math.PI * 2); ctx.fill();
  // wings
  ctx.strokeStyle = PANEL.black.css;
  ctx.lineWidth = 1.4;
  ctx.beginPath();
  ctx.moveTo(-6.5, -1); ctx.quadraticCurveTo(-9, 4, -6, 8);
  ctx.moveTo(6.5, -1); ctx.quadraticCurveTo(9, 4, 6, 8);
  ctx.stroke();
  // feet
  ctx.fillStyle = PANEL.yellow.css;
  ctx.beginPath();
  ctx.moveTo(-3, 11); ctx.lineTo(-6, 13); ctx.lineTo(-1, 12.5); ctx.closePath();
  ctx.moveTo(3, 11); ctx.lineTo(6, 13); ctx.lineTo(1, 12.5); ctx.closePath();
  ctx.fill();
}

function glyphOwl() {
  ctx.fillStyle = PANEL.white.css;
  ctx.strokeStyle = PANEL.black.css;
  ctx.lineWidth = 1.6;
  // ear tufts
  ctx.beginPath();
  ctx.moveTo(-6, -9); ctx.lineTo(-3, -4);
  ctx.moveTo(6, -9); ctx.lineTo(3, -4); ctx.stroke();
  // body
  ctx.beginPath();
  ctx.ellipse(0, 1, 8, 10, 0, 0, Math.PI * 2);
  ctx.fill(); ctx.stroke();
  // facial disc
  ctx.beginPath();
  ctx.arc(-3.5, -2, 4, 0, Math.PI * 2);
  ctx.arc(3.5, -2, 4, 0, Math.PI * 2); ctx.stroke();
  // big eyes
  ctx.fillStyle = PANEL.black.css;
  ctx.beginPath();
  ctx.arc(-3.5, -2, 2.4, 0, Math.PI * 2);
  ctx.arc(3.5, -2, 2.4, 0, Math.PI * 2); ctx.fill();
  ctx.fillStyle = PANEL.white.css;
  ctx.beginPath();
  ctx.arc(-2.8, -2.8, 0.8, 0, Math.PI * 2);
  ctx.arc(4.2, -2.8, 0.8, 0, Math.PI * 2); ctx.fill();
  // yellow beak
  ctx.fillStyle = PANEL.yellow.css;
  ctx.beginPath();
  ctx.moveTo(0, 0); ctx.lineTo(-1.6, 2.6); ctx.lineTo(1.6, 2.6);
  ctx.closePath(); ctx.fill();
  // belly speckles + feet
  ctx.fillStyle = PANEL.black.css;
  for (let i = 0; i < 3; i++) {
    ctx.beginPath();
    ctx.arc(-3 + i * 3, 6, 0.8, 0, Math.PI * 2); ctx.fill();
  }
  ctx.fillStyle = PANEL.yellow.css;
  ctx.fillRect(-3, 10, 1.5, 2.5);
  ctx.fillRect(1.5, 10, 1.5, 2.5);
}

function glyphSloth() {
  ctx.fillStyle = PANEL.white.css;
  ctx.strokeStyle = PANEL.black.css;
  ctx.lineWidth = 1.6;
  // round face
  ctx.beginPath();
  ctx.arc(0, 0, 9, 0, Math.PI * 2);
  ctx.fill(); ctx.stroke();
  // dark eye patches (teardrop)
  ctx.fillStyle = PANEL.black.css;
  ctx.beginPath();
  ctx.ellipse(-3.5, -1, 2.4, 3.4, 0.4, 0, Math.PI * 2); ctx.fill();
  ctx.beginPath();
  ctx.ellipse(3.5, -1, 2.4, 3.4, -0.4, 0, Math.PI * 2); ctx.fill();
  // eyes (white dots in the patches)
  ctx.fillStyle = PANEL.white.css;
  ctx.beginPath();
  ctx.arc(-3.5, -1, 1, 0, Math.PI * 2);
  ctx.arc(3.5, -1, 1, 0, Math.PI * 2); ctx.fill();
  // eye shine
  ctx.fillStyle = PANEL.black.css;
  ctx.beginPath();
  ctx.arc(-3.5, -1, 0.5, 0, Math.PI * 2);
  ctx.arc(3.5, -1, 0.5, 0, Math.PI * 2); ctx.fill();
  // red nose
  ctx.fillStyle = PANEL.red.css;
  ctx.beginPath();
  ctx.moveTo(0, 2); ctx.lineTo(-1.6, 3.4); ctx.lineTo(1.6, 3.4);
  ctx.closePath(); ctx.fill();
  // gentle smile
  ctx.strokeStyle = PANEL.black.css;
  ctx.lineWidth = 1.3;
  ctx.beginPath();
  ctx.arc(0, 4, 2.6, 0.15 * Math.PI, 0.85 * Math.PI); ctx.stroke();
}

function glyphPangolin() {
  ctx.fillStyle = PANEL.white.css;
  ctx.strokeStyle = PANEL.black.css;
  ctx.lineWidth = 1.6;
  // curled body outline
  ctx.beginPath();
  ctx.arc(1, 1, 9, 0, Math.PI * 2);
  ctx.fill(); ctx.stroke();
  // little head poking out (lower left)
  ctx.fillStyle = PANEL.white.css;
  ctx.beginPath();
  ctx.ellipse(-7, 6, 4, 2.6, -0.5, 0, Math.PI * 2);
  ctx.fill(); ctx.stroke();
  ctx.fillStyle = PANEL.black.css;
  ctx.beginPath(); ctx.arc(-8, 5, 0.9, 0, Math.PI * 2); ctx.fill();
  // overlapping scales in a spiral
  ctx.strokeStyle = PANEL.black.css;
  ctx.lineWidth = 1.2;
  for (let ring = 1; ring <= 3; ring++) {
    const rr = ring * 2.6;
    const stepN = 6 + ring * 2;
    for (let s = 0; s < stepN; s++) {
      const a = (s / stepN) * Math.PI * 2;
      const x = 1 + Math.cos(a) * rr;
      const y = 1 + Math.sin(a) * rr;
      ctx.beginPath();
      ctx.arc(x, y, 2, a + 0.4, a + Math.PI - 0.4);
      ctx.stroke();
    }
  }
}

function glyphTiger() {
  ctx.fillStyle = PANEL.yellow.css;
  ctx.strokeStyle = PANEL.black.css;
  ctx.lineWidth = 1.6;
  // ears
  ctx.beginPath();
  ctx.arc(-6, -6, 2.6, 0, Math.PI * 2);
  ctx.arc(6, -6, 2.6, 0, Math.PI * 2);
  ctx.fill(); ctx.stroke();
  ctx.fillStyle = PANEL.red.css;
  ctx.beginPath();
  ctx.arc(-6, -6, 1.1, 0, Math.PI * 2);
  ctx.arc(6, -6, 1.1, 0, Math.PI * 2); ctx.fill();
  // head
  ctx.fillStyle = PANEL.yellow.css;
  ctx.beginPath();
  ctx.arc(0, 0, 9, 0, Math.PI * 2);
  ctx.fill(); ctx.stroke();
  // stripes
  ctx.strokeStyle = PANEL.black.css;
  ctx.lineWidth = 1.6;
  ctx.beginPath();
  ctx.moveTo(0, -9); ctx.lineTo(0, -5);
  ctx.moveTo(-4, -8); ctx.lineTo(-3, -4.5);
  ctx.moveTo(4, -8); ctx.lineTo(3, -4.5);
  ctx.moveTo(-9, -1); ctx.lineTo(-5, 0);
  ctx.moveTo(9, -1); ctx.lineTo(5, 0);
  ctx.moveTo(-8, 4); ctx.lineTo(-4.5, 3.5);
  ctx.moveTo(8, 4); ctx.lineTo(4.5, 3.5);
  ctx.stroke();
  // eyes
  ctx.fillStyle = PANEL.black.css;
  ctx.beginPath();
  ctx.arc(-3.2, -1, 1.4, 0, Math.PI * 2);
  ctx.arc(3.2, -1, 1.4, 0, Math.PI * 2); ctx.fill();
  ctx.fillStyle = PANEL.white.css;
  ctx.beginPath();
  ctx.arc(-2.7, -1.5, 0.5, 0, Math.PI * 2);
  ctx.arc(3.7, -1.5, 0.5, 0, Math.PI * 2); ctx.fill();
  // muzzle + red nose + mouth
  ctx.fillStyle = PANEL.white.css;
  ctx.beginPath(); ctx.ellipse(0, 4, 4, 3, 0, 0, Math.PI * 2); ctx.fill();
  ctx.fillStyle = PANEL.red.css;
  ctx.beginPath();
  ctx.moveTo(0, 3.5); ctx.lineTo(-1.4, 5); ctx.lineTo(1.4, 5);
  ctx.closePath(); ctx.fill();
  ctx.strokeStyle = PANEL.black.css;
  ctx.lineWidth = 1;
  ctx.beginPath();
  ctx.moveTo(0, 5); ctx.lineTo(0, 6.5);
  ctx.moveTo(0, 6.5); ctx.arc(0, 6, 1.4, 0.2 * Math.PI, 0.8 * Math.PI);
  ctx.stroke();
}

// =============================================================
// Compass rose (8-point) + scale bar
// =============================================================

function drawCompass(cx, cy) {
  const R = 15;
  // Ring.
  ctx.fillStyle = PANEL.white.css;
  ctx.beginPath(); ctx.arc(cx, cy, R + 3, 0, Math.PI * 2); ctx.fill();
  ctx.strokeStyle = PANEL.black.css;
  ctx.lineWidth = 1.5;
  ctx.beginPath(); ctx.arc(cx, cy, R + 3, 0, Math.PI * 2); ctx.stroke();

  // Minor (diagonal) points — thin.
  ctx.fillStyle = PANEL.black.css;
  for (let k = 0; k < 4; k++) {
    const a = Math.PI / 4 + k * (Math.PI / 2);
    star4(cx, cy, a, R * 0.7, 2.5);
  }
  // Major points N/E/S/W.
  for (let k = 0; k < 4; k++) {
    const a = k * (Math.PI / 2);
    // North is red.
    ctx.fillStyle = (k === 3) ? PANEL.red.css : PANEL.black.css; // -y is north
    star4(cx, cy, a, R, 3.5);
  }
  // North label.
  ctx.fillStyle = PANEL.black.css;
  ctx.font = "700 9px Georgia, serif";
  ctx.textAlign = "center";
  ctx.fillText("N", cx, cy - R - 5);
  // center hub
  ctx.beginPath(); ctx.arc(cx, cy, 1.6, 0, Math.PI * 2); ctx.fill();
}

// Draw one tapered compass point at angle a (0 = east, grows CW).
function star4(cx, cy, a, len, half) {
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
  // alternating black/white segments
  const seg = 25;
  for (let i = 0; i < 4; i++) {
    if (i % 2 === 0) ctx.fillRect(x + i * seg, y, seg, 3);
  }
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

function drawFooter(animal, box) {
  const x = box.x;
  const y = box.y + box.h - LAYOUT.footerH + 6;

  // Row 1: name (big) + species (italic right).
  ctx.fillStyle = PANEL.black.css;
  ctx.textAlign = "left";
  ctx.font = "700 25px Georgia, serif";
  ctx.fillText(cap(animal.name), x, y + 20);

  ctx.font = "italic 15px Georgia, serif";
  ctx.textAlign = "right";
  ctx.fillText(animal.species, box.x + box.w, y + 20);

  // Row 2: latest-known label + a little blurb, coords on the right.
  ctx.textAlign = "left";
  ctx.font = "13px Georgia, serif";
  ctx.fillText("Latest known location \u2014 " + animal.blurb, x, y + 40);

  ctx.textAlign = "right";
  ctx.font = "12px Georgia, serif";
  ctx.fillText(formatCoords(animal.lat, animal.lng), box.x + box.w, y + 40);

  // Row 3: place name in bold.
  ctx.textAlign = "left";
  ctx.font = "700 16px Georgia, serif";
  ctx.fillText(animal.locationName, x, y + 60);

  // Divider.
  ctx.fillStyle = PANEL.black.css;
  ctx.fillRect(x, y + 68, box.w, 1);

  // Row 4: last signal (red if stale) + distance travelled.
  ctx.font = "13px Georgia, serif";
  ctx.textAlign = "left";
  const stale = animal.lastSignalMin > 60;
  ctx.fillStyle = stale ? PANEL.red.css : PANEL.black.css;
  ctx.fillText("Last signal: " + formatAge(animal.lastSignalMin), x, y + 85);

  ctx.fillStyle = PANEL.black.css;
  ctx.textAlign = "center";
  drawHeart(box.x + box.w / 2 - 44, y + 80, 5);
  ctx.font = "italic 12px Georgia, serif";
  ctx.fillText("Made for Mandy", box.x + box.w / 2 + 6, y + 85);

  ctx.textAlign = "right";
  ctx.font = "13px Georgia, serif";
  ctx.fillText(formatKm(animal.distanceKm) + " travelled", box.x + box.w, y + 85);
}

// =============================================================
// Alternate screens
// =============================================================

function drawBootScreen() {
  fillBackground();
  drawDecorFrame();
  const cx = W / 2;
  ctx.fillStyle = PANEL.black.css;
  ctx.textAlign = "center";
  ctx.font = "700 36px Georgia, serif";
  ctx.fillText("Mandy's Wildlife", cx, H / 2 - 34);
  drawHeart(cx, H / 2 - 8, 12);
  ctx.fillStyle = PANEL.black.css;
  ctx.font = "italic 18px Georgia, serif";
  ctx.fillText("a little tracker, made with love", cx, H / 2 + 30);
  ctx.fillStyle = PANEL.red.css;
  ctx.font = "700 15px Georgia, serif";
  ctx.fillText("Made for Mandy", cx, H / 2 + 62);
}

function drawWiFiScreen() {
  fillBackground();
  drawDecorFrame();
  const cx = W / 2, cy = H / 2 + 40;
  ctx.fillStyle = PANEL.black.css;
  ctx.textAlign = "center";
  ctx.font = "700 26px Georgia, serif";
  ctx.fillText("Finding a signal\u2026", cx, H / 2 - 20);
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
  ctx.fillText("Couldn't reach our wandering friend.", cx, H / 2);
  ctx.fillText("Showing the last known location.", cx, H / 2 + 24);
}

// =============================================================
// Shared drawing helpers
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
  // Presets store NAME in caps; show as Title case for warmth.
  return name.charAt(0) + name.slice(1).toLowerCase();
}

function hashString(str) {
  let h = 2166136261;
  for (let i = 0; i < str.length; i++) {
    h ^= str.charCodeAt(i);
    h = Math.imul(h, 16777619);
  }
  return h >>> 0;
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
// Colour quantisation — force the canvas to the 4 panel colours
// =============================================================

const PALETTE_RGB = [
  [0, 0, 0],       // black
  [255, 255, 255], // white
  [232, 196, 0],   // yellow
  [208, 18, 27],   // red
];

function nearestPaletteIndex(r, g, b) {
  // Luminance-aware mapping. The naive nearest-RGB approach sends
  // dark antialiased edges (black lines over yellow) to RED, which
  // speckles every coastline. Instead: decide black vs. not-black by
  // brightness first, and only pick red for genuinely red-ish pixels.
  const lum = 0.299 * r + 0.587 * g + 0.114 * b;

  // Clearly dark -> black.
  if (lum < 110) return 0;

  // Strong red signal (red dominant, not bright/yellow) -> red.
  if (r > 150 && g < 120 && b < 120 && (r - g) > 60) return 3;

  // Otherwise choose between white and yellow by brightness/hue.
  // Yellow is bright with low blue; white is bright and neutral.
  if (b < 150 && (r + g) / 2 - b > 60) return 2; // yellow
  return 1; // white
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
  const presetKey = document.getElementById("preset").value;
  const screen = document.getElementById("screen").value;
  const animal = PRESETS[presetKey];

  if (screen === "boot") {
    drawBootScreen();
  } else if (screen === "wifi") {
    drawWiFiScreen();
  } else if (screen === "error") {
    drawErrorScreen();
  } else {
    fillBackground();
    drawDecorFrame();
    const box = contentBox();
    drawHeader(animal, box);
    const r = mapRect(box);
    drawMapFrame(r);
    drawMapInterior(r, animal);
    drawFooter(animal, box);
  }

  // Force the image down to the four real panel colours.
  quantiseToPanel();

  if (document.getElementById("grid").checked) {
    drawGridOverlay();
  }
}

// =============================================================
// UI wiring
// =============================================================

function applyScale() {
  const truesize = document.getElementById("truesize").checked;
  const zoom = parseFloat(document.getElementById("zoom").value);
  const scale = truesize ? 1 : zoom;
  canvas.style.width = W * scale + "px";
  canvas.style.height = H * scale + "px";
  document.getElementById("zoom").disabled = truesize;
}

// Allow ?animal=owl&screen=animal in the URL to preselect, which
// makes it easy to preview a specific plate (and to screenshot one).
function applyUrlOverrides() {
  const params = new URLSearchParams(location.search);
  const a = params.get("animal");
  const s = params.get("screen");
  if (a && PRESETS[a]) document.getElementById("preset").value = a;
  if (s) {
    const sel = document.getElementById("screen");
    if ([...sel.options].some((o) => o.value === s)) sel.value = s;
  }
}

function wire() {
  applyUrlOverrides();
  ["preset", "screen", "grid"].forEach((id) =>
    document.getElementById(id).addEventListener("change", render)
  );
  document.getElementById("truesize").addEventListener("change", applyScale);
  document.getElementById("zoom").addEventListener("input", applyScale);

  document.getElementById("download").addEventListener("click", () => {
    const a = document.createElement("a");
    const preset = document.getElementById("preset").value;
    a.download = `mandy-${preset}-648x480.png`;
    a.href = canvas.toDataURL("image/png");
    a.click();
  });

  applyScale();
  render();
}

wire();
