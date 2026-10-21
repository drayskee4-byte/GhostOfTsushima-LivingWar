// Renders the Mongol hierarchy as one self-contained HTML page (no network access needed).
#include "lw/living_war.hpp"

namespace lw {

namespace {
const char* kPage = R"HTML(<!doctype html>
<html lang="en"><head><meta charset="utf-8"><meta name="viewport" content="width=device-width,initial-scale=1">
<title>Living War Hierarchy</title>
<style>
:root{--bg:#f4efe6;--panel:#fffaf1;--ink:#1d1a16;--muted:#6d6457;--line:#d9cfbf;--accent:#a3281f;--gold:#9a7417;--dead:#8b8378;--chip:#efe5d4}
@media (prefers-color-scheme:dark){:root:not([data-theme=light]){--bg:#15130f;--panel:#1f1c17;--ink:#ece4d6;--muted:#a59c8c;--line:#3a342b;--accent:#e0574a;--gold:#d4a73c;--dead:#6f685d;--chip:#2b2620}}
:root[data-theme=dark]{--bg:#15130f;--panel:#1f1c17;--ink:#ece4d6;--muted:#a59c8c;--line:#3a342b;--accent:#e0574a;--gold:#d4a73c;--dead:#6f685d;--chip:#2b2620}
*{box-sizing:border-box}body{margin:0;background:var(--bg);color:var(--ink);font:15px/1.45 Georgia,"Times New Roman",serif}
header{padding:20px 16px 8px;max-width:1200px;margin:auto}h1{margin:0;font-size:26px;letter-spacing:.02em}
.sub{color:var(--muted);font-size:14px}.wrap{max-width:1200px;margin:auto;padding:0 16px 40px;display:grid;grid-template-columns:minmax(0,1.3fr) minmax(0,1fr);gap:16px}
@media (max-width:820px){.wrap{grid-template-columns:1fr}}
.tabs{display:flex;gap:6px;flex-wrap:wrap;margin:12px 0}.tab{border:1px solid var(--line);background:var(--panel);color:var(--ink);padding:6px 12px;border-radius:999px;cursor:pointer;font:inherit;font-size:14px}
.tab[aria-pressed=true]{background:var(--ink);color:var(--bg)}
.panel{background:var(--panel);border:1px solid var(--line);border-radius:10px;padding:14px}
.region-meta{display:flex;gap:14px;flex-wrap:wrap;color:var(--muted);font-size:13px;margin-bottom:10px}
ul.tree{list-style:none;margin:0;padding-left:16px;border-left:1px dashed var(--line)}ul.tree.root{padding-left:0;border:0}
.node{display:flex;align-items:center;gap:8px;padding:5px 8px;margin:3px 0;border-radius:6px;cursor:pointer;flex-wrap:wrap}
.node:hover,.node.sel{background:var(--chip)}.rank{font-size:11px;text-transform:uppercase;letter-spacing:.08em;color:var(--gold);min-width:74px}
.name{font-weight:bold}.dead .name{color:var(--dead);text-decoration:line-through}.badge{font-size:11px;padding:1px 7px;border-radius:999px;background:var(--chip);color:var(--muted)}
.badge.hate{background:var(--accent);color:#fff}.badge.back{border:1px solid var(--accent);color:var(--accent);background:transparent}
h2{font-size:19px;margin:0 0 4px}h3{font-size:13px;text-transform:uppercase;letter-spacing:.08em;color:var(--muted);margin:16px 0 6px}
.bars{display:grid;grid-template-columns:90px 1fr 32px;gap:4px 8px;align-items:center;font-size:13px}.bar{height:8px;background:var(--chip);border-radius:4px;overflow:hidden}.bar i{display:block;height:100%;background:var(--ink)}
.bar.h i{background:var(--accent)}.chips{display:flex;gap:6px;flex-wrap:wrap}.chip{background:var(--chip);border-radius:6px;padding:2px 8px;font-size:13px}
.log{font-size:13px;margin:0;padding-left:18px}.log li{margin:2px 0}.muted{color:var(--muted)}
.events{max-height:320px;overflow:auto}
</style></head><body>
<header><h1>The Mongol Occupation</h1><div class="sub" id="sub"></div><div class="tabs" id="tabs"></div></header>
<div class="wrap"><section class="panel"><div class="region-meta" id="rmeta"></div><div id="tree"></div></section>
<aside class="panel" id="detail"><p class="muted">Select an officer to see his history.</p></aside>
<section class="panel" style="grid-column:1/-1"><h3>Recent events</h3><ul class="log events" id="events"></ul></section></div>
<script>
const W = /*DATA*/;
const C = {}; W.commanders.forEach(c => C[c.id] = c);
const RANKS = {arban:"Arban",jaghun:"Jaghun",mingghan:"Mingghan",tumen:"Tumen",noyan:"Noyan"};
const TITLE = {arban:"Arban-u Noyan",jaghun:"Jaghun-u Noyan",mingghan:"Mingghan-u Noyan",tumen:"Tumen-u Noyan",noyan:"Khan's Noyan"};
const esc = s => String(s ?? "").replace(/[&<>"]/g, ch => ({"&":"&amp;","<":"&lt;",">":"&gt;",'"':"&quot;"}[ch]));
const nm = c => c.name + (c.epithet ? " " + c.epithet : "");
let region = (W.regions.find(r => r.unlocked) || W.regions[0]).id, sel = null, showDead = false;
document.getElementById("sub").textContent = `Day ${W.day} · Act ${W.act} · ${W.commanders.filter(c=>c.alive&&!c.storyProtected).length} officers alive`;
function tabs(){const t=document.getElementById("tabs");t.innerHTML="";
 W.regions.forEach(r=>{const b=document.createElement("button");b.className="tab";b.textContent=r.name+(r.unlocked?"":" (locked)");b.setAttribute("aria-pressed",r.id===region);b.onclick=()=>{region=r.id;render()};t.appendChild(b)});
 const d=document.createElement("button");d.className="tab";d.textContent=showDead?"Hide the fallen":"Show the fallen";d.onclick=()=>{showDead=!showDead;render()};t.appendChild(d)}
function badges(c){let b="";if(!c.alive)b+=`<span class="badge">fell day ${c.diedOn}</span>`;else{
 if(c.presumedDead)b+=`<span class="badge back">presumed dead</span>`;
 if(c.hate>=W.config.hateVengefulAt)b+=`<span class="badge hate">${c.hate>=W.config.hateMurderousAt?"murderous":"vengeful"}</span>`;
 if(c.stats.encounters)b+=`<span class="badge">met Jin ×${c.stats.encounters}</span>`;
 if(c.stats.timesCheatedDeath)b+=`<span class="badge">cheated death ×${c.stats.timesCheatedDeath}</span>`}return b}
function branch(id,depth){const kids=W.commanders.filter(c=>c.superior===id&&(c.alive||showDead)&&(c.region===region||c.rank==="noyan"));
 if(!kids.length||depth>8)return"";kids.sort((a,b)=>Object.keys(RANKS).indexOf(b.rank)-Object.keys(RANKS).indexOf(a.rank));
 return `<ul class="tree${depth?"":" root"}">`+kids.map(c=>`<li><div class="node ${c.alive?"":"dead"} ${sel===c.id?"sel":""}" data-id="${c.id}"><span class="rank">${RANKS[c.rank]}</span><span class="name">${esc(nm(c))}</span>${badges(c)}</div>${branch(c.id,depth+1)}</li>`).join("")+"</ul>"}
function render(){tabs();const r=W.regions.find(x=>x.id===region);
 document.getElementById("rmeta").innerHTML=`<span>Mongol control <b>${r.control}%</b></span><span>Pressure ${r.pressure}</span><span>Alert ${r.alert}</span>`;
 const top=region==="iki"?"the_eagle":"khotun_khan";const root=C[top];
 let html=`<div class="node"><span class="rank">Story</span><span class="name">${esc(root.name)}</span><span class="badge">protected</span></div>`;
 if(region!=="iki"){const noyans=W.commanders.filter(c=>c.rank==="noyan"&&!c.storyProtected&&(c.alive||showDead));
  html+=`<ul class="tree">`+noyans.map(n=>`<li><div class="node ${n.alive?"":"dead"} ${sel===n.id?"sel":""}" data-id="${n.id}"><span class="rank">Noyan</span><span class="name">${esc(nm(n))}</span>${badges(n)}</div>${branch(n.id,1)}</li>`).join("")+"</ul>"}
 else html+=branch(top,1);
 const orphans=W.commanders.filter(c=>c.region===region&&!c.storyProtected&&(c.alive||showDead)&&c.rank!=="noyan"&&!C[c.superior]);
 if(orphans.length)html+=`<h3>Unattached</h3>`+orphans.map(c=>`<div class="node" data-id="${c.id}"><span class="rank">${RANKS[c.rank]}</span><span class="name">${esc(nm(c))}</span>${badges(c)}</div>`).join("");
 const tree=document.getElementById("tree");tree.innerHTML=html;tree.querySelectorAll("[data-id]").forEach(n=>n.onclick=()=>{sel=n.dataset.id;render();detail()});
 document.getElementById("events").innerHTML=W.events.slice().reverse().filter(e=>!e.region||e.region===region||e.kind==="seed").slice(0,60).map(e=>`<li><span class="muted">Day ${e.day}</span> ${esc(e.text)}</li>`).join("")}
function bar(label,v,cls){return `<span>${label}</span><span class="bar ${cls||""}"><i style="width:${v}%"></i></span><span>${v}</span>`}
function detail(){const c=C[sel];if(!c)return;const sup=C[c.superior];
 const ties=c.ties.filter(t=>C[t.other]).map(t=>`<span class="chip">${t.type.replace("_"," ")}: ${esc(nm(C[t.other]))}${C[t.other].alive?"":" †"}</span>`).join("");
 const facets=Object.entries(c.facets).map(([k,v])=>bar(k,v)).join("");
 const list=(a,f)=>a.length?`<ul class="log">${a.map(f).join("")}</ul>`:`<p class="muted">None.</p>`;
 document.getElementById("detail").innerHTML=`<h2>${esc(nm(c))}</h2><div class="muted">${TITLE[c.rank]} · ${esc(c.archetype)} · ${esc(c.region)} · from ${esc(c.origin)}</div>
 ${c.alive?"":`<p><b>Fate:</b> ${esc(c.fate)}</p>`}${sup?`<p class="muted">Answers to ${esc(nm(sup))}</p>`:""}
 ${c.goal&&c.goal.type!=="none"?`<p><b>Wants to</b> ${esc(c.goal.text)}</p>`:""}
 <h3>Toward Jin</h3><div class="bars">${bar("Hate",c.hate,"h")}${bar("Fear",c.fearOfJin)}</div>
 <h3>Learned tactics</h3><div class="chips">${c.adaptations.map(a=>`<span class="chip" title="${esc(a.reason)}">${a.id.replace("_"," ")} ${"I".repeat(a.level)}</span>`).join("")||'<span class="muted">None yet.</span>'}</div>
 <h3>Traits</h3><div class="chips">${[...c.strengths.map(s=>"+"+s),...c.weaknesses.map(s=>"−"+s),...c.fears.map(s=>"fears "+s.replace("_"," "))].map(s=>`<span class="chip">${esc(s)}</span>`).join("")||'<span class="muted">Unremarkable.</span>'}</div>
 <h3>Scars</h3>${list(c.scars,s=>`<li>${esc(s.text)} <span class="muted">(day ${s.day})</span></li>`)}
 <h3>Memories</h3>${list(c.memories.slice().reverse(),m=>`<li><b>${esc(m.emotion)}</b> ${esc(m.text)}</li>`)}
 <h3>Ties</h3><div class="chips">${ties||'<span class="muted">None.</span>'}</div>
 <h3>Personality</h3><div class="bars">${facets}</div>
 <h3>Record</h3><p class="muted">Met Jin ${c.stats.encounters}× · beaten by him ${c.stats.defeatsByJin}× · beat him ${c.stats.victoriesOverJin}× · fled ${c.stats.timesFled}× · spared ${c.stats.timesSpared}× · men lost ${c.stats.menLostToJin}</p>
 <h3>History</h3>${list(c.history.slice().reverse(),h=>`<li><span class="muted">Day ${h.day}</span> ${esc(h.text)}</li>`)}`}
render();
</script></body></html>
)HTML";
} // namespace

std::string renderHierarchyHtml(const World& w) {
    std::string page = kPage;
    std::string data = w.toJson().dump();
    // Never let data close the script tag.
    std::string safe;
    safe.reserve(data.size());
    for (size_t i = 0; i < data.size(); ++i) {
        if (data[i] == '<' && i + 1 < data.size() && data[i + 1] == '/') { safe += "<\\/"; ++i; }
        else safe += data[i];
    }
    auto pos = page.find("/*DATA*/");
    page.replace(pos, 8, safe);
    return page;
}

} // namespace lw
