import { createClient } from 'https://cdn.jsdelivr.net/npm/@supabase/supabase-js@2.45.4/+esm';

/* ---------- Constants ---------- */
const I = {
  film:'<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2"><rect x="3" y="4" width="18" height="16" rx="2"/><path d="M7 4v16M17 4v16M3 9h4M3 15h4M17 9h4M17 15h4"/></svg>',
  tv:'<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2"><rect x="3" y="6" width="18" height="12" rx="2"/><path d="M8 21h8M9 2l3 4 3-4"/></svg>',
  book:'<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2"><path d="M3 5c3-1 6-1 9 1v14c-3-2-6-2-9-1zM21 5c-3-1-6-1-9 1v14c3-2 6-2 9-1z"/></svg>',
  music:'<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2"><circle cx="12" cy="12" r="9"/><circle cx="12" cy="12" r="2.5"/></svg>',
  heart:'<svg viewBox="0 0 24 24" fill="currentColor"><path d="M12 21s-7.5-4.6-9.6-9.3C.9 8.2 3 4.5 6.6 4.5c2.1 0 3.5 1.1 5.4 3.1 1.9-2 3.3-3.1 5.4-3.1 3.6 0 5.7 3.7 4.2 7.2C19.5 16.4 12 21 12 21z"/></svg>',
  heartO:'<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2"><path d="M12 20s-7.5-4.6-9.6-9.3C.9 7.2 3 3.5 6.6 3.5c2.1 0 3.5 1.1 5.4 3.1 1.9-2 3.3-3.1 5.4-3.1 3.6 0 5.7 3.7 4.2 7.2C19.5 15.4 12 20 12 20z"/></svg>',
  rew:'<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2.4" stroke-linecap="round"><path d="M4 12a8 8 0 1 0 3-6.2M4 4v4h4"/></svg>',
  home:'<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linejoin="round"><path d="M3 11l9-7 9 7v9a1 1 0 0 1-1 1h-5v-6h-6v6H4a1 1 0 0 1-1-1z"/></svg>',
  search:'<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2.2" stroke-linecap="round"><circle cx="11" cy="11" r="7"/><path d="M20 20l-4-4"/></svg>',
  plus:'<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2.6" stroke-linecap="round"><path d="M12 5v14M5 12h14"/></svg>',
  bolt:'<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linejoin="round"><path d="M13 2L4 14h7l-1 8 9-12h-7z"/></svg>',
  user:'<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2"><circle cx="12" cy="8" r="4"/><path d="M4 21c1-4 4.5-6 8-6s7 2 8 6"/></svg>',
  back:'<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2.4" stroke-linecap="round" stroke-linejoin="round"><path d="M15 5l-7 7 7 7"/></svg>',
  gear:'<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2"><circle cx="12" cy="12" r="3"/><path d="M19.4 15a1.7 1.7 0 0 0 .3 1.8l.1.1a2 2 0 1 1-2.8 2.8l-.1-.1a1.7 1.7 0 0 0-1.8-.3 1.7 1.7 0 0 0-1 1.5V21a2 2 0 1 1-4 0v-.1a1.7 1.7 0 0 0-1.1-1.5 1.7 1.7 0 0 0-1.8.3l-.1.1a2 2 0 1 1-2.8-2.8l.1-.1a1.7 1.7 0 0 0 .3-1.8 1.7 1.7 0 0 0-1.5-1H3a2 2 0 1 1 0-4h.1a1.7 1.7 0 0 0 1.5-1.1 1.7 1.7 0 0 0-.3-1.8l-.1-.1a2 2 0 1 1 2.8-2.8l.1.1a1.7 1.7 0 0 0 1.8.3H9a1.7 1.7 0 0 0 1-1.5V3a2 2 0 1 1 4 0v.1a1.7 1.7 0 0 0 1 1.5 1.7 1.7 0 0 0 1.8-.3l.1-.1a2 2 0 1 1 2.8 2.8l-.1.1a1.7 1.7 0 0 0-.3 1.8V9a1.7 1.7 0 0 0 1.5 1H21a2 2 0 1 1 0 4h-.1a1.7 1.7 0 0 0-1.5 1z"/></svg>',
  up:'<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2.4" stroke-linecap="round" stroke-linejoin="round"><path d="M6 15l6-6 6 6"/></svg>',
  down:'<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2.4" stroke-linecap="round" stroke-linejoin="round"><path d="M6 9l6 6 6-6"/></svg>',
  x:'<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2.4" stroke-linecap="round"><path d="M6 6l12 12M18 6L6 18"/></svg>',
  eye:'<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2"><path d="M2 12s3.5-7 10-7 10 7 10 7-3.5 7-10 7S2 12 2 12z"/><circle cx="12" cy="12" r="3"/></svg>',
  clock:'<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2"><circle cx="12" cy="12" r="9"/><path d="M12 7v5l3 2"/></svg>',
  list:'<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2.2" stroke-linecap="round"><path d="M8 6h13M8 12h13M8 18h13M3 6h.01M3 12h.01M3 18h.01"/></svg>',
  pin:'<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linejoin="round"><path d="M9 3h6l-1 6 4 4H6l4-4zM12 13v8"/></svg>',
  chat:'<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linejoin="round"><path d="M4 5h16v11H9l-5 4z"/></svg>',
  disc:'<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2"><rect x="4" y="3" width="16" height="18" rx="2"/><circle cx="12" cy="12" r="3"/></svg>',
  copy:'<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2"><rect x="8" y="8" width="13" height="13" rx="2"/><path d="M16 8V5a2 2 0 0 0-2-2H5a2 2 0 0 0-2 2v9a2 2 0 0 0 2 2h3"/></svg>',
  pinloc:'<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2"><path d="M12 21s-7-6.2-7-12a7 7 0 0 1 14 0c0 5.8-7 12-7 12z"/><circle cx="12" cy="9" r="2.5"/></svg>',
  link:'<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round"><path d="M10 14a5 5 0 0 0 7 0l3-3a5 5 0 0 0-7-7l-1 1M14 10a5 5 0 0 0-7 0l-3 3a5 5 0 0 0 7 7l1-1"/></svg>',
  out:'<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M14 4h6v6M20 4l-9 9M18 14v5a1 1 0 0 1-1 1H5a1 1 0 0 1-1-1V7a1 1 0 0 1 1-1h5"/></svg>',
  upload:'<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round"><path d="M12 16V4M6 10l6-6 6 6M4 20h16"/></svg>'
};
const LOGO = '<svg viewBox="0 0 512 512" aria-hidden="true"><path d="M256 92 L118 412 M256 92 L394 412" fill="none" stroke="currentColor" stroke-width="58" stroke-linecap="round" stroke-linejoin="round"/><g stroke-width="26" stroke-linecap="round"><line x1="191" y1="350" x2="201" y2="350" stroke="var(--film)"/><line x1="231" y1="350" x2="241" y2="350" stroke="var(--tv)"/><line x1="271" y1="350" x2="281" y2="350" stroke="var(--book)"/><line x1="311" y1="350" x2="321" y2="350" stroke="var(--music)"/></g><path d="M372 76 l13 31 31 13 -31 13 -13 31 -13 -31 -31 -13 31 -13z" fill="var(--accent)"/></svg>';
const TYPES = {
  film:{label:'Film',plural:'Films',verb:'watched',creator:'Director',ph:'e.g. Past Lives'},
  tv:{label:'TV',plural:'Shows',verb:'watched',creator:'Creator',ph:'e.g. Severance, Season 2'},
  book:{label:'Book',plural:'Books',verb:'read',creator:'Author',ph:'e.g. Piranesi'},
  music:{label:'Music',plural:'Albums',verb:'listened to',creator:'Artist',ph:'e.g. Blonde'}
};
const TK = Object.keys(TYPES);
const SERVICES = ['Theater','Netflix','Max','Hulu','Disney+','Prime Video','Apple TV+','Peacock','Paramount+','Criterion Channel','MUBI','YouTube','Spotify','Apple Music','Vinyl','Kindle','Library','Physical copy'];

/* ---------- State ---------- */
const S = {db:null,user:null,assets:null,uid:null,canWrite:false,ready:false,booted:false,
  prof:{},items:{},subs:{},plat:{},
  stack:[{v:'home'}], ftype:'all', fkind:'all', favType:'film', statsYear:'all', dType:'all', dOwned:false, dSvc:'', wSvc:'', wType:'all',
  q:'', qType:'all', cEdit:null, cDel:null, homeTab:'explore', explore:null, follows:[], remote:null, assets:{delete:async()=>{}}};

/* ---------- Helpers ---------- */
const $ = s => document.querySelector(s);
const esc = s => String(s ?? '').replace(/[&<>"']/g,c=>({'&':'&amp;','<':'&lt;','>':'&gt;','"':'&quot;',"'":'&#39;'}[c]));
const today = () => new Date().toLocaleDateString('en-CA');
const thisYear = () => String(new Date().getFullYear());
const items = uid => S.items[uid] || [];
const allItems = () => Object.values(S.items).flat();
const byDate = (a,b) => (b.date||'').localeCompare(a.date||'') || (b.createdAt||0)-(a.createdAt||0);
const ents = uid => items(uid).filter(i=>i.kind==='entry').sort(byDate);
const allEnts = () => allItems().filter(i=>i.kind==='entry').sort(byDate);
const lists = uid => items(uid).filter(i=>i.kind==='list').sort((a,b)=>(b.updatedAt||0)-(a.updatedAt||0));
const watch = uid => items(uid).filter(i=>i.kind==='watch').sort((a,b)=>(a.rank||0)-(b.rank||0));
const find = (uid,id) => items(uid).find(i=>i.id===id);
const tkey = e => (e.type||'film')+'|'+String(e.title||'').trim().toLowerCase()+'|'+(e.year||'');
const people = () => [...new Set([...(S.uid?[S.uid]:[]), ...Object.keys(S.prof), ...Object.keys(S.items).filter(u=>items(u).length)])];
const prof = uid => S.prof[uid] || {};
const nm = uid => { const p=prof(uid); return p.handle || p.display || 'member'; };
const dname = uid => { const p=prof(uid); return p.display || p.handle || 'member'; };
function initials(uid){ const n=dname(uid); const c=['#FF7A59','#34C3AE','#6EA8FF','#B98BFF','#F2B33D'][[...String(uid)].reduce((a,ch)=>a+ch.charCodeAt(0),0)%5];
  const ch=(n.trim()[0]||'?').toUpperCase().replace(/[^A-Z0-9]/,'•');
  return 'data:image/svg+xml,'+encodeURIComponent(`<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 64 64"><rect width="64" height="64" fill="${c}"/><text x="32" y="42" font-family="system-ui,sans-serif" font-size="28" font-weight="700" text-anchor="middle" fill="#14171E">${ch}</text></svg>`); }
const av = uid => prof(uid).avatar || initials(uid);
const blob = u => u;
function stars(r){ if(!r) return ''; const h=Math.round(r*2)/2; return '★'.repeat(Math.floor(h))+(h%1?'½':''); }
function rating(r){ if(!r) return ''; const exact=Math.abs(r*2-Math.round(r*2))>1e-6; return `<span class="stars" aria-label="${r} out of 5">${stars(r)}</span>${exact?`<span class="score">${r.toFixed(1)}</span>`:''}`; }
const fmtDate = (d,o={month:'short',day:'numeric',year:'numeric'}) => { if(!d) return ''; const [y,m,dd]=d.split('-').map(Number); return new Date(y,m-1,dd||1).toLocaleDateString(undefined,o); };
const ago = t => { const s=(Date.now()-t)/1000; if(s<60) return 'now'; if(s<3600) return Math.floor(s/60)+'m'; if(s<86400) return Math.floor(s/3600)+'h'; if(s<604800) return Math.floor(s/86400)+'d'; return new Date(t).toLocaleDateString(undefined,{month:'short',day:'numeric'}); };
const cur = () => S.stack[S.stack.length-1];
const isMe = uid => uid && uid===S.uid;
const comments = (owner,eid) => allItems().filter(i=>i.kind==='comment'&&i.to===owner&&i.entry===eid).sort((a,b)=>(a.createdAt||0)-(b.createdAt||0));
const likes = (owner,eid) => allItems().filter(i=>i.kind==='like'&&i.to===owner&&i.entry===eid);
function cover(e){
  const t=TYPES[e.type]?e.type:'film';
  if(e.cover) return `<div class="cv" data-type="${t}"><img src="${blob(esc(e.cover))}" alt="" loading="lazy"></div>`;
  return `<div class="cv" data-type="${t}"><div class="gc"><span class="gct">${esc(e.title||'')}</span><span class="gcc">${esc(e.creator||'')}</span>${I[t]}</div></div>`;
}
function extLink(x){
  if(x.type!=='music') return '';
  const q=[x.creator,x.title].filter(Boolean).join(' ');
  return `<a class="btn ext" href="https://www.albumoftheyear.org/search/?q=${encodeURIComponent(q)}" target="_blank" rel="noopener">${I.out} Album of the Year</a>`;
}
function safeUrl(u){ u=String(u||'').trim(); if(!u) return ''; if(!/^https?:\/\//i.test(u)) u='https://'+u; try{ const x=new URL(u); return /^https?:$/.test(x.protocol)?x.href:''; }catch(e){ return ''; } }
function avimg(uid,cls=''){ const s=av(uid); return `<img class="${cls}" data-av="${esc(uid)}" src="${esc(s)}" alt="">`; }

/* ---------- Navigation ---------- */
function go(r){ S.stack.push(r); S.cEdit=null; S.cDel=null; render(true); }
function back(){ if(S.stack.length>1) S.stack.pop(); render(true); }
function tab(r){ S.stack=[r]; render(true); }

/* ---------- Render ---------- */
function render(scrollTop){
  if(!S.uid){ $('#tb').innerHTML=`<div></div><div class="logo">${LOGO}<span>Artistry</span></div><div></div>`; $('#bnav').innerHTML=''; document.body.classList.add('signed-out'); $('#main').innerHTML=signInV(); return; }
  document.body.classList.remove('signed-out');
  const r = cur();
  let title='', left='', right='';
  const backBtn = `<button class="iconbtn" data-a="back" aria-label="Back">${I.back}</button>`;
  if(S.stack.length>1) left=backBtn;
  let html='';
  switch(r.v){
    case 'home': title='<div class="logo">'+LOGO+'<span>Artistry</span></div>'; html=homeV(); break;
    case 'search': title='<h1>Search</h1>'; html=searchV(); break;
    case 'activity': title='<h1>Activity</h1>'; html=activityV(); break;
    case 'profile': { const u=r.u||S.uid; title=`<h1>${esc(u?nm(u):'Profile')}</h1>`; if(isMe(u)&&S.canWrite) right=`<button class="iconbtn" data-a="edit-profile" aria-label="Edit profile">${I.gear}</button>`; html=profileV(u, r.t||'profile'); break; }
    case 'entry': title='<h1>'+esc(TYPES[(find(r.u,r.i)||{}).type]?.label||'Entry')+'</h1>'; html=entryV(r.u,r.i); break;
    case 'list': title='<h1>List</h1>'; html=listV(r.u,r.i); break;
    case 'title': title='<h1>'+esc(TYPES[r.k.split('|')[0]]?.label||'Title')+'</h1>'; html=titleV(r.k); break;
    case 'section': { const sec=S.explore&&S.explore.sections&&S.explore.sections[r.n]; title='<h1>'+esc(sec?sec.title:'Explore')+'</h1>'; html=xsectionV(r.n); break; }
  }
  $('#tb').innerHTML = `<div>${left}</div>${title.startsWith('<h1')?title:`<div>${title}</div>`}<div class="r">${right}</div>`;
  const root=S.stack[0].v;
  const unread = notifs().filter(n=>n.t>(lastSeen()||0)).length;
  $('#bnav').innerHTML = [
    ['home','Home',I.home],['search','Search',I.search],['log','Log',I.plus],['activity','Activity',I.bolt],['profile','Profile',I.user]
  ].map(([k,l,ic])=> k==='log'
    ? `<button class="logbtn" data-a="log" aria-label="Log something"${S.canWrite?'':' hidden'}><span>${ic}</span></button>`
    : `<button data-a="nav" data-k="${k}" aria-current="${root===k?'page':'false'}">${ic}${k==='activity'&&unread?'<i class="dot"></i>':''}${l}</button>`).join('');
  const m=$('#main'); m.innerHTML = html;
  if(scrollTop){ window.scrollTo(0,0); m.classList.remove('view'); void m.offsetWidth; m.classList.add('view'); }
  if(r.v==='activity') markSeen();
  if(r.v==='search'){ const q=$('#q'); if(q && document.activeElement!==q && S.focusQ){ q.focus(); S.focusQ=false; } }
  refreshProfiles();
}

/* ----- Home ----- */
function homeV(){
  const tabs=`<div class="hometabs" role="tablist"><button role="tab" data-a="htab" data-k="explore" aria-selected="${S.homeTab==='explore'}">Explore</button><button role="tab" data-a="htab" data-k="friends" aria-selected="${S.homeTab==='friends'}">Friends</button></div>`;
  return tabs + (S.homeTab==='explore' ? exploreV() : friendsV());
}
function coverIndex(){ const m=new Map(); allItems().forEach(i=>{ if(i.cover && i.title){ const k=(i.type||'film')+'|'+i.title.trim().toLowerCase(); if(!m.has(k)) m.set(k,i.cover); } (i.items||[]).forEach(x=>{ if(x.cover&&x.title){ const k=(x.type||'film')+'|'+x.title.trim().toLowerCase(); if(!m.has(k)) m.set(k,x.cover);} }); }); return m; }
function exploreItems(){ const out=[]; ((S.explore&&S.explore.sections)||[]).forEach(sec=>(sec.items||[]).forEach(it=>out.push({...it,type:sec.type}))); if(S.explore&&S.explore.spotlight) out.push(S.explore.spotlight); return out; }
function exploreFind(k){ return S.catalog.get(k) || exploreItems().find(x=>tkey(x)===k); }
const CHEV='<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2.4" stroke-linecap="round" stroke-linejoin="round"><path d="M9 5l7 7-7 7"/></svg>';
function xhead(title,attrs){ return `<button class="xhead" ${attrs}><h2>${esc(title)}</h2>${CHEV}</button>`; }
function exploreV(){
  const X=S.explore; const ci=coverIndex();
  const withCover=it=>({...it,cover:it.cover||ci.get(it.type+'|'+String(it.title).trim().toLowerCase())||null});
  let h='';
  if(!X){ h+= S.exploreErr ? `<div class="empty"><h3>Charts didn’t load</h3><p>${esc(S.exploreErr)}</p></div>` : `<div class="empty"><p>Loading charts…</p></div>`; }
  else (X.sections||[]).forEach((sec,n)=>{
    if(!(sec.items||[]).length) return;
    h+=xhead(sec.title,`data-a="xsection" data-n="${n}"`)+`<div class="hrow xrow" data-type="${esc(sec.type)}">${sec.items.map(it=>{ const e=withCover({...it,type:sec.type}); return `<button class="pcell" data-a="title" data-k="${esc(tkey(e))}" aria-label="${esc(e.title)}">${cover(e)}</button>`; }).join('')}</div>`;
  });
  const others=allEnts().filter(e=>!isMe(e.uid)&&inCircle(e.uid));
  if(others.length) h+=xhead('New from friends','data-a="htab" data-k="friends"')+`<div class="hrow xrow">${others.slice(0,12).map(e=>`<button class="pcell" data-a="entry" data-u="${esc(e.uid)}" data-i="${esc(e.id)}">${cover(e)}<span class="who2">${avimg(e.uid,'avs')}<span>${esc(nm(e.uid))}</span></span><span class="under">${rating(e.rating)}${e.liked?`<span class="heart">${I.heart}</span>`:''}${e.review?`<span class="rew">${I.list}</span>`:''}</span></button>`).join('')}</div>`;
  const pop=popular().filter(t=>t.uids.size>1).slice(0,12);
  if(pop.length) h+=xhead('Popular with friends','data-a="htab" data-k="friends"')+`<div class="hrow xrow">${pop.map(t=>`<button class="pcell" data-a="title" data-k="${esc(t.key)}" aria-label="${esc(t.title)}">${cover(t)}</button>`).join('')}</div>`;
  h+=`<p class="foot">Charts refresh hourly. Movie and TV data from TMDB. This product uses the TMDB API but is not endorsed or certified by TMDB. Books from Open Library; music charts from Apple Music.</p>`;
  return h;
}
function xsectionV(n){
  const sec=S.explore&&S.explore.sections&&S.explore.sections[n]; if(!sec) return `<div class="empty"><p>Loading…</p></div>`;
  const ci=coverIndex(); const tm=titleMap();
  return `<p class="ghost" style="margin:14px 0 4px">${esc(sec.source||'')}</p><div class="grid4 xgrid" data-type="${esc(sec.type)}">${sec.items.map(it=>{ const e={...it,type:sec.type,cover:it.cover||ci.get(sec.type+'|'+String(it.title).trim().toLowerCase())||null}; const t=tm.get(tkey(e));
    return `<button class="pcell" data-a="title" data-k="${esc(tkey(e))}">${cover(e)}<span class="ttl">${esc(e.title)}</span><span class="sub">${t&&t.avg?`<span class="stars">${stars(t.avg)}</span>`:esc(e.creator||e.year||'')}</span></button>`; }).join('')}</div>`;
}
function friendsV(){
  const es = allEnts();
  if(!S.ready) return `<div class="empty"><p>Loading…</p></div>`;
  if(!es.length && !allItems().some(i=>i.kind==='list')) return `<div class="empty">
    <h3>Start your diary</h3>
    <p>Log the films, shows, books and albums you finish. Friends you invite show up here as they log too.</p>
    <div class="acts">${S.canWrite?`<button class="btn pri" data-a="log">${I.plus} Log something</button><button class="btn" data-a="import">${I.upload} Import from Letterboxd</button>`:''}</div></div>`;
  const others = es.filter(e=>!isMe(e.uid)&&inCircle(e.uid));
  const fresh = (others.length?others:es).slice(0,12);
  let h = `<div class="sec"><h2>New from ${others.length?'friends':'your diary'}</h2></div>
  <div class="hrow">${fresh.map(e=>`<button class="pcell" data-a="entry" data-u="${esc(e.uid)}" data-i="${esc(e.id)}">${cover(e)}<span class="under">${avimg(e.uid,'avs')}${rating(e.rating)}${e.liked?`<span class="heart">${I.heart}</span>`:''}</span></button>`).join('')}</div>`;
  // popular with friends
  const pop = popular().filter(t=>t.uids.size>1).slice(0,8);
  if(pop.length) h += `<div class="sec"><h2>Popular with friends</h2></div><div class="hrow">${pop.map(t=>`<button class="pcell" data-a="title" data-k="${esc(t.key)}">${cover(t)}<span class="under ghost">${t.uids.size} logged · ${t.avg?stars(t.avg):'–'}</span></button>`).join('')}</div>`;
  h += `<div class="sec"><h2>Activity</h2></div>` + chipsType('ftype') +
    `<div class="chips" style="padding-top:8px">${[['all','Everything'],['review','Reviews'],['loved','Loved'],['list','Lists']].map(([k,l])=>`<button class="chip" data-a="fkind" data-k="${k}" aria-pressed="${S.fkind===k}">${l}</button>`).join('')}</div>`;
  let feed = [];
  allItems().forEach(i=>{
    if(!inCircle(i.uid)) return;
    if(S.ftype!=='all' && i.kind==='entry' && i.type!==S.ftype) return;
    if(i.kind==='entry'){ if(S.fkind==='list') return; if(S.fkind==='review'&&!i.review) return; if(S.fkind==='loved'&&!i.liked) return; feed.push(i); }
    else if(i.kind==='list'){ if(S.fkind==='all'||S.fkind==='list'){ if(S.ftype==='all'||(i.items||[]).some(x=>x.type===S.ftype)) feed.push(i);} }
  });
  feed.sort((a,b)=>sortT(b)-sortT(a));
  h += feed.slice(0,80).map(feedCard).join('') || `<p class="ghost" style="padding:20px 0">Nothing here with these filters.</p>`;
  return h;
}
const sortT = i => i.kind==='entry' ? Math.max(i.createdAt||0, Date.parse(i.date||'')||0) : (i.updatedAt||i.createdAt||0);
function chipsType(key){
  return `<div class="chips">${[['all','All']].concat(TK.map(k=>[k,TYPES[k].plural])).map(([k,l])=>`<button class="chip" ${k!=='all'?`data-type="${k}"`:''} data-a="ftype" data-key="${key}" data-k="${k}" aria-pressed="${S[key]===k}">${k!=='all'?'<i></i>':''}${l}</button>`).join('')}</div>`;
}
function feedCard(i){
  if(i.kind==='list'){
    const its=(i.items||[]).slice(0,5);
    return `<div class="card listcard" data-a="list" data-u="${esc(i.uid)}" data-i="${esc(i.id)}" style="grid-template-columns:minmax(0,1fr)">
      <div><div class="who">${avimg(i.uid)}<span><b>${esc(nm(i.uid))}</b> ${i.cloneOf?'cloned':'made'} a list</span></div>
      <h3>${esc(i.name)}</h3><div class="meta"><span>${(i.items||[]).length} items</span>${i.ranked?'<span>Ranked</span>':''}</div>
      ${i.desc?`<p class="review" style="-webkit-line-clamp:2">${esc(i.desc)}</p>`:''}
      <div class="strip">${its.map(cover).join('')}</div></div></div>`;
  }
  const t=TYPES[i.type]||TYPES.film; const lc=likes(i.uid,i.id).length, cc=comments(i.uid,i.id).length;
  return `<div class="card" data-type="${esc(i.type)}" data-a="entry" data-u="${esc(i.uid)}" data-i="${esc(i.id)}">
    <div style="min-width:0"><div class="who">${avimg(i.uid)}<span><b>${esc(nm(i.uid))}</b> ${i.rewatch?'re':''}${t.verb}</span></div>
      <h3>${esc(i.title)}${i.year?`<span class="yr">${esc(i.year)}</span>`:''}</h3>
      <div class="meta">${rating(i.rating)}${i.liked?`<span class="heart">${I.heart}</span>`:''}${i.rewatch?`<span class="rew">${I.rew}</span>`:''}<span class="mono" style="font-size:11.5px">${fmtDate(i.date,{month:'short',day:'numeric'})}</span><span class="tag">${t.label}</span></div>
      ${i.review?`<p class="review ${i.spoilers?'spoil':''}">${esc(i.review)}</p>${i.spoilers?'<div class="spoilnote">Contains spoilers</div>':''}`:''}
      ${lc||cc?`<div class="counts">${lc?`<span>${I.heart}${lc}</span>`:''}${cc?`<span>${I.chat}${cc}</span>`:''}</div>`:''}
    </div>${cover(i)}</div>`;
}

/* ----- Titles (aggregated across everyone) ----- */
function titleMap(){
  const m=new Map();
  allItems().forEach(i=>{
    if(i.kind!=='entry' && i.kind!=='watch') return;
    const k=tkey(i); let t=m.get(k);
    if(!t){ t={key:k,type:i.type,title:i.title,year:i.year,creator:i.creator,cover:null,logs:[],wants:[],uids:new Set()}; m.set(k,t); }
    if(!t.cover && i.cover) t.cover=i.cover; if(!t.creator && i.creator) t.creator=i.creator;
    if(i.kind==='entry'){ t.logs.push(i); t.uids.add(i.uid); } else t.wants.push(i);
  });
  m.forEach(t=>{ const r=t.logs.filter(l=>l.rating); t.avg=r.length? r.reduce((s,l)=>s+l.rating,0)/r.length : 0; });
  return m;
}
function popular(){ return [...titleMap().values()].sort((a,b)=>b.uids.size-a.uids.size || b.avg-a.avg); }

function titleV(k){
  const t = titleMap().get(k);
  if(!t){ const x=exploreFind(k); if(!x) return `<div class="empty"><h3>No one has logged this yet</h3></div>`;
    const ci=coverIndex(); const xc={...x,cover:x.cover||ci.get(x.type+'|'+String(x.title).trim().toLowerCase())||null};
    if(x.ext && !x._det && /^tmdb:/.test(x.ext)){ x._det=true; fetchDetails(x.ext).then(d=>{ if(d&&d.creator){ x.creator=d.creator; if(cur().v==='title') render(); } }); }
    return (x.backdrop?`<div class="ebanner" style="background-image:url('${esc(x.backdrop)}')"></div>`:'')+`<div class="ehead" data-type="${esc(x.type)}"><div class="bd" ${xc.cover?`style="background-image:url('${blob(esc(xc.cover))}')"`:'style="background:var(--t)"'}></div><div class="in">
    <div><span class="tag">${TYPES[x.type].label}</span><h2>${esc(x.title)}</h2><div class="cr">${[x.year,x.creator].filter(Boolean).map(esc).join(' · ')}</div></div>${cover(xc)}</div></div>
    ${x.note?`<p class="ghost" style="margin-top:12px">${esc(x.note)}</p>`:''}
    <div class="facts"><span class="ghost">No one in Artistry has logged this yet.</span></div>
    ${S.canWrite?`<div class="actbar"><button class="btn pri" data-a="log-x" data-k="${esc(k)}">${I.plus} Be the first to log it</button><button class="btn" data-a="watch-x" data-k="${esc(k)}">${I.clock} Watchlist</button><button class="btn" data-a="to-list" data-k="${esc(k)}">${I.list} Add to list</button>${extLink(x)}</div>`:(extLink(x)?`<div class="actbar">${extLink(x)}</div>`:'')}`; }
  const ratedLogs=t.logs.filter(l=>l.rating); const bins=Array(10).fill(0); ratedLogs.forEach(l=>bins[Math.max(0,Math.min(9,Math.round(l.rating*2)-1))]++);
  const mine = t.logs.find(l=>isMe(l.uid)); const inWatch = watch(S.uid).find(w=>tkey(w)===k);
  return `<div class="ehead" data-type="${esc(t.type)}"><div class="bd" ${t.cover?`style="background-image:url('${blob(esc(t.cover))}')"`:''}></div><div class="in">
    <div><span class="tag">${TYPES[t.type].label}</span><h2>${esc(t.title)}</h2><div class="cr">${[t.year,t.creator].filter(Boolean).map(esc).join(' · ')}</div></div>${cover(t)}</div></div>
    <div class="facts"><span>${t.uids.size} ${t.uids.size===1?'friend':'friends'} logged</span>${t.avg?`<span>Friends' average ${rating(Math.round(t.avg*10)/10)}</span>`:''}${t.wants.length?`<span class="ghost">${t.wants.length} want to</span>`:''}</div>
    ${S.canWrite?`<div class="actbar">${mine?`<button class="btn" data-a="entry" data-u="${esc(mine.uid)}" data-i="${esc(mine.id)}">Your log</button>`:''}
      <button class="btn pri" data-a="log-title" data-k="${esc(k)}">${I.plus} ${mine?'Log again':'Log it'}</button>
      ${inWatch?`<button class="btn" disabled>${I.clock} On watchlist</button>`:`<button class="btn" data-a="watch-title" data-k="${esc(k)}">${I.clock} Watchlist</button>`}
      <button class="btn" data-a="to-list" data-k="${esc(k)}">${I.list} Add to list</button>${extLink(t)}</div>`:(extLink(t)?`<div class="actbar">${extLink(t)}</div>`:'')}
    ${ratedLogs.length>1?`<div class="sec"><h2>Ratings from friends</h2></div>${histHTML(bins)}`:''}
    <div class="sec"><h2>Logs</h2></div>${t.logs.sort(byDate).map(feedCard).join('')}`;
}
function histHTML(bins){ const max=Math.max(1,...bins); return `<div class="histw"><span class="s">★</span><div class="hist">${bins.map((n,i)=>`<div class="${n?'':'z'}" style="height:${n?Math.max(8,n/max*100):3}%" title="${stars((i+1)/2)}: ${n}"></div>`).join('')}</div><span class="s">★★★★★</span></div>`; }

/* ----- Search ----- */
function searchV(){
  const q=S.q.trim().toLowerCase();
  let h=`<div class="searchbox">${I.search}<input id="q" type="search" placeholder="Titles, people, creators, reviews" value="${esc(S.q)}" autocomplete="off" enterkeyhint="search"></div>`+chipsType('qType');
  const tm=[...titleMap().values()].filter(t=>S.qType==='all'||t.type===S.qType);
  if(!q){
    const pop=tm.sort((a,b)=>b.uids.size-a.uids.size||b.avg-a.avg).slice(0,20);
    h+=`<div class="sec"><h2>Most logged by friends</h2></div>`+(pop.map(trow).join('')||`<p class="ghost">Nothing logged yet.</p>`);
    h+=`<div class="sec"><h2>People</h2></div>`+people().map(prow).join('');
    return h;
  }
  const ts=tm.filter(t=>t.title.toLowerCase().includes(q)||(t.creator||'').toLowerCase().includes(q)||(t.year||'')===q).sort((a,b)=>b.uids.size-a.uids.size).slice(0,40);
  const ps=people().filter(u=>nm(u).toLowerCase().includes(q));
  const rs=allEnts().filter(e=>(e.review||'').toLowerCase().includes(q)&&(S.qType==='all'||e.type===S.qType)).slice(0,20);
  const R=S.remote; const logged=new Set(ts.map(t=>t.key));
  const cat=(R&&R.results||[]).filter(x=>!logged.has(tkey(x)));
  if(ts.length) h+=`<div class="sec"><h2>Logged by friends</h2><span class="ghost">${ts.length}</span></div>`+ts.map(trow).join('');
  h+=`<div class="sec"><h2>Everything</h2>${R&&R.loading?'<span class="ghost">Searching…</span>':''}</div>`+
    (R&&R.err?`<p class="ghost">${esc(R.err)}</p>`:'')+cat.map(crow).join('')+
    (R&&!R.loading&&!cat.length&&!ts.length?`<p class="ghost">No matches.${S.canWrite?` <button class="linkbtn" data-a="log-new" data-t="${esc(S.q)}">Log “${esc(S.q)}” anyway</button>`:''}</p>`:'')+
    (R&&R.tmdb===false&&(S.qType==='all'||S.qType==='film'||S.qType==='tv')?`<p class="hint">Film and TV search needs a TMDB key on the server.</p>`:'');
  if(ps.length) h+=`<div class="sec"><h2>People</h2></div>`+ps.map(prow).join('');
  if(rs.length) h+=`<div class="sec"><h2>Reviews</h2></div>`+rs.map(feedCard).join('');
  return h;
}
function trow(t){
  const inW = S.uid && watch(S.uid).some(w=>tkey(w)===t.key);
  return `<div class="trow" data-type="${esc(t.type)}"><div data-a="title" data-k="${esc(t.key)}" style="cursor:pointer">${cover(t)}</div>
    <div class="t" data-a="title" data-k="${esc(t.key)}"><b>${esc(t.title)}${t.year?`<span class="yr">${esc(t.year)}</span>`:''}</b><small><span class="tag">${TYPES[t.type].label}</span> ${esc(t.creator||'')} · ${t.uids.size} logged${t.avg?' · '+stars(t.avg):''}</small></div>
    ${S.canWrite?`<div class="q"><button class="iconbtn" data-a="log-title" data-k="${esc(t.key)}" aria-label="Log ${esc(t.title)}">${I.plus}</button><button class="iconbtn" data-a="watch-title" data-k="${esc(t.key)}" aria-label="Add to watchlist" ${inW?'disabled style="opacity:.4"':''}>${I.clock}</button></div>`:''}</div>`;
}
function crow(x){
  const k=tkey(x); const inW = S.uid && watch(S.uid).some(w=>tkey(w)===k);
  return `<div class="trow" data-type="${esc(x.type)}"><div data-a="title" data-k="${esc(k)}" style="cursor:pointer">${cover(x)}</div>
    <div class="t" data-a="title" data-k="${esc(k)}"><b>${esc(x.title)}${x.year?`<span class="yr">${esc(x.year)}</span>`:''}</b><small><span class="tag">${TYPES[x.type].label}</span> ${esc(x.creator||'')}</small></div>
    ${S.canWrite?`<div class="q"><button class="iconbtn" data-a="log-x" data-k="${esc(k)}" aria-label="Log ${esc(x.title)}">${I.plus}</button><button class="iconbtn" data-a="watch-x" data-k="${esc(k)}" aria-label="Add to watchlist" ${inW?'disabled style="opacity:.4"':''}>${I.clock}</button></div>`:''}</div>`;
}
function prow(u){ const n=ents(u).length; return `<button class="prow" data-a="profile" data-u="${esc(u)}">${avimg(u)}<span><b>${esc(dname(u))}${isMe(u)?' (you)':''}</b><small>@${esc(nm(u))} · ${n} logged${!isMe(u)&&isFollowing(u)?' · Following':''}</small></span></button>`; }

/* ----- Activity (notifications) ----- */
function notifs(){
  if(!S.uid) return [];
  const out=[];
  allItems().forEach(i=>{
    if(i.uid===S.uid) return;
    if((i.kind==='comment'||i.kind==='like') && i.to===S.uid){ const e=find(S.uid,i.entry); if(e) out.push({t:i.createdAt||0,i,e}); }
    if(i.kind==='list' && i.cloneOf && i.cloneOf.u===S.uid){ out.push({t:i.createdAt||0,i,clone:true}); }
  });
  (S.follows||[]).filter(f=>f.followee===S.uid).forEach(f=>out.push({t:Date.parse(f.created_at)||0,follow:f.follower}));
  return out.sort((a,b)=>b.t-a.t);
}
function lastSeen(){ try{ return +localStorage.getItem('artistry-seen')||0; }catch(e){ return 0; } }
function markSeen(){ try{ localStorage.setItem('artistry-seen', String(Date.now())); }catch(e){} }
function activityV(){
  const ns=notifs();
  let h=`<div class="sec"><h2>On your logs</h2></div>`;
  h+= ns.map(n=>{
    if(n.follow) return `<div class="nrow" data-a="profile" data-u="${esc(n.follow)}">${avimg(n.follow)}<p><b>${esc(dname(n.follow))}</b> started following you<small>${ago(n.t)}</small></p><span></span></div>`;
    if(n.clone) return `<div class="nrow" data-a="list" data-u="${esc(n.i.uid)}" data-i="${esc(n.i.id)}">${avimg(n.i.uid)}<p><b data-nm="${esc(n.i.uid)}">${esc(nm(n.i.uid))}</b> cloned your list ${esc(n.i.cloneOf.name||'')}<small>${ago(n.t)}</small></p><span></span></div>`;
    const verb = n.i.kind==='like' ? 'liked your review of' : 'commented on';
    return `<div class="nrow" data-a="entry" data-u="${esc(S.uid)}" data-i="${esc(n.e.id)}">${avimg(n.i.uid)}<p><b data-nm="${esc(n.i.uid)}">${esc(nm(n.i.uid))}</b> ${verb} <b>${esc(n.e.title)}</b>${n.i.kind==='comment'?`<small style="color:var(--ink)">${esc(n.i.text.slice(0,140))}</small>`:''}<small>${ago(n.t)}</small></p>${cover(n.e)}</div>`;
  }).join('') || `<p class="ghost">Likes, comments and new followers show up here.</p>`;
  const fr = allEnts().filter(e=>!isMe(e.uid)&&inCircle(e.uid)).sort((a,b)=>(b.createdAt||0)-(a.createdAt||0)).slice(0,30);
  h+=`<div class="sec"><h2>Friends lately</h2></div>`+(fr.map(e=>`<div class="nrow" data-a="entry" data-u="${esc(e.uid)}" data-i="${esc(e.id)}">${avimg(e.uid)}<p><b data-nm="${esc(e.uid)}">${esc(nm(e.uid))}</b> ${TYPES[e.type]?.verb||'logged'} <b>${esc(e.title)}</b> ${rating(e.rating)}<small>${ago(e.createdAt||0)}</small></p>${cover(e)}</div>`).join('')||`<p class="ghost">Follow people from Search to see what they log.</p>`);
  return h;
}

/* ----- Profile ----- */
function profileV(u,t){
  if(!u) return `<div class="empty"><h3>Sign in to get a profile</h3></div>`;
  const p=prof(u), es=ents(u), mine=isMe(u)&&S.canWrite;
  const yr=es.filter(e=>(e.date||'').startsWith(thisYear())).length;
  const fallBd = es.find(e=>e.backdrop)?.backdrop || es.find(e=>e.cover)?.cover;
  const url=safeUrl(p.link); const host=url?new URL(url).host.replace(/^www\./,''):'';
  let h=`<section class="hero ${p.backdrop?'has-banner':''}">${p.backdrop?`<div class="banner" style="background-image:url('${esc(blob(p.backdrop))}')"></div>`:`<div class="bd" ${fallBd?`style="background-image:url('${esc(blob(fallBd))}')"`:''}></div>`}
    ${avimg(u,'bigav')}<h2>${esc(dname(u))}</h2><div class="ghost">@${esc(nm(u))}</div>
    ${p.location||url?`<div class="where">${p.location?`<span>${I.pinloc}${esc(p.location)}</span>`:''}${url?`<a href="${esc(url)}" target="_blank" rel="noopener">${I.link}${esc(host)}</a>`:''}</div>`:''}
    ${p.bio?`<p class="bio">${esc(p.bio)}</p>`:(mine?`<p class="bio"><button class="linkbtn" data-a="edit-profile">Add a bio, photo and banner</button></p>`:'')}
    <div class="actbar" style="justify-content:center">${!isMe(u)&&S.canWrite?`<button class="btn ${isFollowing(u)?'':'pri'} sm" data-a="follow" data-u="${esc(u)}">${isFollowing(u)?'Following':'Follow'}</button>`:''}<button class="btn sm" data-a="share-profile" data-u="${esc(u)}">${I.link} Share</button></div>
    <div class="statrow"><button data-a="ptab" data-t="diary"><b>${es.length}</b><span>Logged</span></button><button data-a="ptab" data-t="diary"><b>${yr}</b><span>${thisYear()}</span></button><button data-a="noop"><b>${followerCount(u)}</b><span>Followers</span></button><button data-a="noop"><b>${followingCount(u)}</b><span>Following</span></button></div></section>
    <div class="ptabs" role="tablist">${[['profile','Profile'],['diary','Diary'],['lists','Lists'],['watch','Watchlist'],['stats','Stats']].map(([k,l])=>`<button role="tab" data-a="ptab" data-t="${k}" aria-selected="${t===k}">${l}</button>`).join('')}</div>`;
  if(t==='diary') return h+diaryV(u);
  if(t==='lists') return h+listsV(u);
  if(t==='watch') return h+watchV(u);
  if(t==='stats') return h+statsV(u);
  // Profile tab
  const pins=(p.pins||[]).map(x=>({...x,obj:find(u,x.id)})).filter(x=>x.obj);
  if(pins.length) h+=`<div class="sec"><h2>Pinned</h2></div><div class="pinned">${pins.map(x=>x.obj.kind==='list'
    ? `<button class="pin" data-a="list" data-u="${esc(u)}" data-i="${esc(x.id)}">${cover((x.obj.items||[])[0]||{title:x.obj.name,type:'film'})}<span><b>${esc(x.obj.name)}</b><small>List · ${(x.obj.items||[]).length} items</small></span></button>`
    : `<button class="pin" data-a="entry" data-u="${esc(u)}" data-i="${esc(x.id)}">${cover(x.obj)}<span><b>${esc(x.obj.title)}</b><small>${rating(x.obj.rating)} ${x.obj.review?'· Review':''}</small></span></button>`).join('')}</div>`;
  const ft=S.favType; const favIds=((p.favs||{})[ft]||[]); const favs=favIds.map(id=>find(u,id)).filter(Boolean);
  h+=`<div class="sec"><h2>Favorites</h2><div class="typesw">${TK.map(k=>`<button data-type="${k}" data-a="favtype" data-k="${k}" aria-pressed="${ft===k}">${TYPES[k].plural}</button>`).join('')}</div></div>
    <div class="grid4">${[0,1,2,3].map(n=>favs[n]?`<button class="pcell" data-a="entry" data-u="${esc(u)}" data-i="${esc(favs[n].id)}">${cover(favs[n])}</button>`:(mine?`<button class="slot" data-a="fav-pick" aria-label="Choose favorite">${I.plus}</button>`:`<div class="slot" aria-hidden="true"></div>`)).join('')}</div>`;
  const rec=es.slice(0,4);
  h+=`<div class="sec"><h2>Recent activity</h2>${es.length>4?`<button class="more" data-a="ptab" data-t="diary">All ${es.length}</button>`:''}</div>`+
    (rec.length?`<div class="grid4">${rec.map(e=>`<button class="pcell" data-a="entry" data-u="${esc(u)}" data-i="${esc(e.id)}">${cover(e)}<span class="under">${rating(e.rating)}${e.liked?`<span class="heart">${I.heart}</span>`:''}${e.review?`<span class="rew" title="Has review">${I.list}</span>`:''}</span></button>`).join('')}</div>`:`<p class="ghost">${mine?'Nothing logged yet. Tap the + button to log your first.':'Nothing logged yet.'}</p>`);
  const rated=es.filter(e=>e.rating); if(rated.length){ const bins=Array(10).fill(0); rated.forEach(e=>bins[Math.max(0,Math.min(9,Math.round(e.rating*2)-1))]++); h+=`<div class="sec"><h2>Ratings</h2><span class="ghost">${rated.length}</span></div>${histHTML(bins)}`; }
  return h;
}

function diaryV(u){
  let es=ents(u).filter(e=>(S.dType==='all'||e.type===S.dType)&&(!S.dOwned||e.owned)&&(!S.dSvc||e.service===S.dSvc));
  const svcs=[...new Set(ents(u).map(e=>e.service).filter(Boolean))];
  let h=chipsType('dType')+`<div class="chips" style="padding-top:8px"><button class="chip" data-a="dOwned" aria-pressed="${S.dOwned}">${I.disc.replace('<svg','<svg width="14" height="14"')} In collection</button>${svcs.length?'<span class="sep"></span>':''}${svcs.map(s=>`<button class="chip" data-a="dSvc" data-k="${esc(s)}" aria-pressed="${S.dSvc===s}">${esc(s)}</button>`).join('')}</div>`;
  if(!es.length) return h+`<p class="ghost" style="padding:24px 0">No entries match.</p>`;
  let curM='';
  es.forEach(e=>{ const m=(e.date||'').slice(0,7); if(m!==curM){ curM=m; const n=es.filter(x=>(x.date||'').startsWith(m)).length; h+=`<div class="month"><span>${fmtDate(m+'-01',{month:'long',year:'numeric'})}</span><span>${n}</span></div>`; }
    h+=`<button class="drow" data-type="${esc(e.type)}" data-a="entry" data-u="${esc(u)}" data-i="${esc(e.id)}"><span class="day">${(e.date||'').slice(8,10)}</span>${cover(e)}<span class="t">${esc(e.title)}<small>${esc(e.year||'')}</small></span><span class="r">${rating(e.rating)}${e.liked?`<span class="heart">${I.heart}</span>`:''}${e.rewatch?`<span class="rew">${I.rew}</span>`:''}</span></button>`; });
  return h;
}

function listsV(u){
  const ls=lists(u); const mine=isMe(u)&&S.canWrite;
  let h= mine?`<div style="margin-top:16px"><button class="btn pri" data-a="new-list">${I.plus} New list</button></div>`:'';
  h+= ls.map(l=>feedCard({...l})).join('') || `<p class="ghost" style="padding:24px 0">No lists yet.</p>`;
  return h;
}

function watchV(u){
  const mine=isMe(u)&&S.canWrite;
  let ws=watch(u); const svcs=[...new Set(ws.map(w=>w.service).filter(Boolean))];
  let h=`<div class="chips">${[['all','All']].concat(TK.map(k=>[k,TYPES[k].plural])).map(([k,l])=>`<button class="chip" ${k!=='all'?`data-type="${k}"`:''} data-a="ftype" data-key="wType" data-k="${k}" aria-pressed="${S.wType===k}">${k!=='all'?'<i></i>':''}${l}</button>`).join('')}</div>`;
  if(svcs.length) h+=`<div class="chips" style="padding-top:8px"><span class="ghost" style="flex:none;align-self:center">Available on</span>${svcs.map(s=>`<button class="chip" data-a="wSvc" data-k="${esc(s)}" aria-pressed="${S.wSvc===s}">${esc(s)}</button>`).join('')}</div>`;
  if(mine) h+=`<div style="margin-top:12px"><button class="btn" data-a="watch-add">${I.plus} Add to watchlist</button></div>`;
  const shown=ws.filter(w=>(S.wType==='all'||w.type===S.wType)&&(!S.wSvc||w.service===S.wSvc));
  if(!shown.length) return h+`<p class="ghost" style="padding:24px 0">${ws.length?'Nothing matches these filters.':'Watchlist is empty. Add things from Search or a title page.'}</p>`;
  h+=shown.map((w)=>{ const n=ws.indexOf(w)+1; return `<div class="wrow" data-type="${esc(w.type)}"><span class="n">${n}</span><div data-a="title" data-k="${esc(tkey(w))}" style="cursor:pointer">${cover(w)}</div>
    <div class="t">${esc(w.title)}<small><span class="tag">${TYPES[w.type]?.label||''}</span> ${esc([w.year,w.creator].filter(Boolean).join(' · '))}</small>${w.service?`<span class="svc">${esc(w.service)}</span>`:''}</div>
    ${mine?`<div class="acts"><button class="iconbtn" data-a="w-up" data-i="${esc(w.id)}" aria-label="Move up" ${n===1?'disabled style="opacity:.3"':''}>${I.up}</button><button class="iconbtn" data-a="w-down" data-i="${esc(w.id)}" aria-label="Move down" ${n===ws.length?'disabled style="opacity:.3"':''}>${I.down}</button><button class="btn sm" data-a="w-log" data-i="${esc(w.id)}">Log</button><button class="iconbtn" data-a="w-del" data-i="${esc(w.id)}" aria-label="Remove">${I.x}</button></div>`:''}</div>`; }).join('');
  return h;
}

function statsV(u){
  const all=ents(u); const years=[...new Set(all.map(e=>(e.date||'').slice(0,4)).filter(Boolean))].sort().reverse();
  if(S.statsYear!=='all' && !years.includes(S.statsYear)) S.statsYear='all';
  let h=`<div class="chips"><button class="chip" data-a="syear" data-k="all" aria-pressed="${S.statsYear==='all'}">All time</button>${years.map(y=>`<button class="chip" data-a="syear" data-k="${y}" aria-pressed="${S.statsYear===y}">${y}</button>`).join('')}</div>`;
  const es = S.statsYear==='all'? all : all.filter(e=>(e.date||'').startsWith(S.statsYear));
  if(!es.length) return h+`<p class="ghost" style="padding:24px 0">No entries yet.</p>`;
  const rated=es.filter(e=>e.rating); const avg=rated.length?(rated.reduce((s,e)=>s+e.rating,0)/rated.length).toFixed(2):'–';
  h+=`<div class="sec"><h2>${S.statsYear==='all'?'All time':S.statsYear}</h2></div><div class="sgrid">${TK.map(k=>`<div data-type="${k}"><b>${es.filter(e=>e.type===k).length}</b><span><i></i>${TYPES[k].plural}</span></div>`).join('')}</div>
    <div class="sgrid" style="grid-template-columns:repeat(3,minmax(0,1fr));margin-top:10px"><div><b>${es.length}</b><span>Logged</span></div><div><b>${es.filter(e=>e.review).length}</b><span>Reviews</span></div><div><b>${avg}</b><span>Avg rating</span></div></div>`;
  // bars by month or year
  let labels, groups;
  if(S.statsYear==='all'){ labels=years.slice().reverse().slice(-10); groups=labels.map(y=>es.filter(e=>(e.date||'').startsWith(y))); }
  else { labels=['J','F','M','A','M','J','J','A','S','O','N','D']; groups=labels.map((_,i)=>es.filter(e=>+(e.date||'').slice(5,7)===i+1)); }
  const mx=Math.max(1,...groups.map(g=>g.length)); const cols=`grid-template-columns:repeat(${labels.length},1fr)`;
  h+=`<div class="sec"><h2>${S.statsYear==='all'?'By year':'By month'}</h2></div><div class="bars" style="${cols}">${groups.map(g=>`<div style="height:${g.length/mx*100}%" title="${g.length}">${TK.map(k=>{const n=g.filter(e=>e.type===k).length; return n?`<i data-type="${k}" style="height:${n/g.length*100}%"></i>`:'';}).join('')}</div>`).join('')}</div><div class="barlbl" style="${cols}">${labels.map(l=>`<span>${S.statsYear==='all'?"'"+l.slice(2):l}</span>`).join('')}</div>`;
  if(rated.length){ const bins=Array(10).fill(0); rated.forEach(e=>bins[Math.max(0,Math.min(9,Math.round(e.rating*2)-1))]++); h+=`<div class="sec"><h2>Rating spread</h2></div>${histHTML(bins)}`; }
  // top creators
  const cr={}; es.forEach(e=>{ if(!e.creator) return; const k=e.type+'|'+e.creator; (cr[k]=cr[k]||{type:e.type,name:e.creator,n:0,r:[]}).n++; if(e.rating) cr[k].r.push(e.rating); });
  const topc=Object.values(cr).filter(c=>c.n>1).sort((a,b)=>b.n-a.n).slice(0,6);
  if(topc.length) h+=`<div class="sec"><h2>Most logged creators</h2></div><div class="rank">${topc.map(c=>`<div data-type="${c.type}"><b><span class="tag">${TYPES[c.type].label}</span> ${esc(c.name)}</b><span>${c.n} · ${c.r.length?stars(c.r.reduce((a,b)=>a+b,0)/c.r.length):''}</span></div>`).join('')}</div>`;
  // above/below friends' average
  const tm=titleMap(); const diffs=[];
  rated.forEach(e=>{ const t=tm.get(tkey(e)); if(!t) return; const o=t.logs.filter(l=>l.uid!==u&&l.rating); if(!o.length) return; const fa=o.reduce((s,l)=>s+l.rating,0)/o.length; diffs.push({e,d:e.rating-fa}); });
  if(diffs.length){ const hi=diffs.filter(x=>x.d>0).sort((a,b)=>b.d-a.d).slice(0,4), lo=diffs.filter(x=>x.d<0).sort((a,b)=>a.d-b.d).slice(0,4);
    const row=x=>`<div data-type="${esc(x.e.type)}" data-a="entry" data-u="${esc(u)}" data-i="${esc(x.e.id)}" style="cursor:pointer"><b>${esc(x.e.title)}</b><span class="delta ${x.d>0?'up':'dn'}">${x.d>0?'+':''}${x.d.toFixed(1)}★</span></div>`;
    if(hi.length) h+=`<div class="sec"><h2>Rated above friends</h2></div><div class="rank">${hi.map(row).join('')}</div>`;
    if(lo.length) h+=`<div class="sec"><h2>Rated below friends</h2></div><div class="rank">${lo.map(row).join('')}</div>`; }
  const top=[...rated].sort((a,b)=>b.rating-a.rating||(b.liked?1:0)-(a.liked?1:0)).slice(0,8);
  if(top.length) h+=`<div class="sec"><h2>Highest rated</h2></div><div class="grid4">${top.map(e=>`<button class="pcell" data-a="entry" data-u="${esc(u)}" data-i="${esc(e.id)}">${cover(e)}<span class="under">${rating(e.rating)}</span></button>`).join('')}</div>`;
  return h;
}

/* ----- Entry detail ----- */
function entryV(u,id){
  const e=find(u,id); if(!e) return `<div class="empty"><h3>This entry was deleted</h3></div>`;
  const t=TYPES[e.type]||TYPES.film; const mine=isMe(u)&&S.canWrite;
  const lk=likes(u,id); const iLike=lk.some(l=>isMe(l.uid)); const cs=comments(u,id);
  const p=prof(u); const pinned=(p.pins||[]).some(x=>x.id===id); const fav=((p.favs||{})[e.type]||[]).includes(id);
  let h=(e.backdrop?`<div class="ebanner" style="background-image:url('${blob(esc(e.backdrop))}')"></div>`:'')+`<div class="ehead" data-type="${esc(e.type)}"><div class="bd" ${e.backdrop?'hidden':''} ${e.cover?`style="background-image:url('${blob(esc(e.cover))}')"`:`style="background:var(--t)"`}></div><div class="in">
    <div><button class="byline" data-a="profile" data-u="${esc(u)}">${avimg(u)}<span><b data-nm="${esc(u)}">${esc(nm(u))}</b> ${e.rewatch?'re':''}${t.verb}</span></button>
    <h2 data-a="title" data-k="${esc(tkey(e))}" style="cursor:pointer">${esc(e.title)}</h2><div class="cr">${[e.year,e.creator].filter(Boolean).map(esc).join(' · ')}</div></div>${cover(e)}</div></div>
    <div class="facts">${rating(e.rating)}${e.liked?`<span class="heart" style="font-size:0">${I.heart.replace('<svg','<svg width="20" height="20"')}</span>`:''}<span class="mono ghost">${fmtDate(e.date)}</span></div>
    <div class="toggles" style="margin-top:4px"><span class="tag" style="align-self:center">${t.label}</span>${e.rewatch?`<span class="pill">${I.rew}Again</span>`:''}${e.service?`<span class="pill">${esc(e.service)}</span>`:''}${e.owned?`<span class="pill">${I.disc}In collection</span>`:''}${(e.tags||[]).map(x=>`<span class="pill">#${esc(x)}</span>`).join('')}</div>`;
  if(e.review) h+= e.spoilers ? `<div class="fullreview spoil" data-a="unspoil" title="Tap to reveal">${esc(e.review)}</div><div class="spoilnote">Contains spoilers. Tap to reveal.</div>` : `<div class="fullreview">${esc(e.review)}</div>`;
  h+=`<div class="actbar">${S.canWrite&&!isMe(u)?`<button class="btn likebtn" data-a="like" aria-pressed="${iLike}">${iLike?I.heart:I.heartO} ${iLike?'Liked':'Like'}${lk.length?` · ${lk.length}`:''}</button>`:(lk.length?`<span class="pill">${I.heart} ${lk.length} ${lk.length===1?'like':'likes'}</span>`:'')}
    ${mine?`<button class="btn" data-a="edit-entry">Edit</button><button class="btn" data-a="pin-entry">${I.pin} ${pinned?'Unpin':'Pin'}</button><button class="btn" data-a="fav-entry">${fav?I.heart:I.heartO} ${fav?'Favorite':'Make favorite'}</button>`:''}
    ${S.canWrite?`<button class="btn" data-a="to-list" data-k="${esc(tkey(e))}">${I.list} Add to list</button>`:''}
    <button class="btn" data-a="title" data-k="${esc(tkey(e))}">Everyone's logs</button>${extLink(e)}</div>`;
  h+=`<div class="sec"><h2>Comments</h2><span class="ghost">${cs.length||''}</span></div><div class="comments">`+cs.map(c=>{
    const own=isMe(c.uid)&&S.canWrite;
    if(S.cEdit===c.id) return `<div class="cmt">${avimg(c.uid)}<div><textarea id="cedit" maxlength="1500">${esc(c.text)}</textarea><div class="own"><button class="linkbtn" data-a="c-save" data-i="${esc(c.id)}">Save</button><button class="linkbtn" style="color:var(--muted)" data-a="c-cancel">Cancel</button></div></div></div>`;
    return `<div class="cmt">${avimg(c.uid)}<div><div class="h"><b data-nm="${esc(c.uid)}">${esc(nm(c.uid))}</b><span>${ago(c.createdAt||0)}${c.editedAt?' · edited':''}</span></div><p>${esc(c.text)}</p>${own?`<div class="own"><button class="linkbtn" data-a="c-edit" data-i="${esc(c.id)}">Edit</button><button class="linkbtn" style="color:var(--danger)" data-a="c-del" data-i="${esc(c.id)}">${S.cDel===c.id?'Tap again to delete':'Delete'}</button></div>`:''}</div></div>`;
  }).join('')+`</div>`;
  if(S.canWrite) h+=`<div class="composer"><textarea id="cnew" maxlength="1500" placeholder="Add a comment" rows="1"></textarea><button class="btn pri" data-a="c-post">Post</button></div>`;
  return h;
}

/* ----- List detail ----- */
function listV(u,id){
  const l=find(u,id); if(!l) return `<div class="empty"><h3>This list was deleted</h3></div>`;
  const mine=isMe(u)&&S.canWrite; const its=l.items||[]; const pinned=(prof(u).pins||[]).some(x=>x.id===id);
  let h=`<div style="padding-top:18px"><button class="byline" data-a="profile" data-u="${esc(u)}">${avimg(u)}<span>List by <b data-nm="${esc(u)}">${esc(nm(u))}</b></span></button>
    <h2 style="font-family:var(--f-display);font-size:28px;margin:10px 0 4px;line-height:1.1">${esc(l.name)}</h2>
    ${l.cloneOf?`<div class="ghost">Cloned from ${esc(l.cloneOf.name||'a list')}</div>`:''}
    ${l.desc?`<p style="white-space:pre-wrap;color:var(--muted);max-width:60ch">${esc(l.desc)}</p>`:''}
    <div class="actbar">${mine?`<button class="btn" data-a="edit-list" data-i="${esc(id)}">Edit list</button><button class="btn" data-a="pin-list" data-i="${esc(id)}">${I.pin} ${pinned?'Unpin':'Pin'}</button>`:''}${S.canWrite?`<button class="btn" data-a="clone-list" data-u="${esc(u)}" data-i="${esc(id)}">${I.copy} Clone</button>`:''}</div></div>`;
  if(!its.length) return h+`<p class="ghost" style="padding:24px 0">This list is empty.</p>`;
  h+=`<div class="grid4" style="margin-top:18px">${its.map((x,n)=>`<button class="pcell" data-a="title" data-k="${esc(tkey(x))}">${cover(x)}<span class="under">${l.ranked?`<b class="mono" style="color:var(--muted)">${n+1}</b>`:''}<span style="overflow:hidden;text-overflow:ellipsis">${esc(x.title)}</span></span></button>`).join('')}</div>`;
  return h;
}

/* ---------- UI utils ---------- */
function toast(m){ const t=$('#toast'); t.textContent=m; t.hidden=false; clearTimeout(toast.t); toast.t=setTimeout(()=>t.hidden=true,2600); }
function notice(m){ const n=$('#notice'); n.textContent=m||''; n.hidden=!m; }
function refreshProfiles(){}

/* ---------- Supabase data layer ---------- */
let sb=null;
const TABLE={entry:'entries',comment:'comments',like:'likes',list:'lists',watch:'watchlist'};
const iso=t=>new Date(t).toISOString();
function toRow(kind,b){
  switch(kind){
    case 'entry': return {type:b.type,title:b.title,creator:b.creator||'',year:b.year||'',date:b.date,rating:b.rating||0,liked:!!b.liked,rewatch:!!b.rewatch,owned:!!b.owned,spoilers:!!b.spoilers,
      service:b.service||'',tags:b.tags||[],review:b.review||'',poster_url:b.cover||null,backdrop_url:b.backdrop||null,ext_id:b.ext||null,updated_at:iso(Date.now())};
    case 'comment': return {entry_id:b.entry,text:b.text,edited_at:b.editedAt?iso(b.editedAt):null};
    case 'list': return {name:b.name,description:b.desc||'',ranked:!!b.ranked,items:b.items||[],clone_of:b.cloneOf?.i||null,clone_of_user:b.cloneOf?.u||null,clone_of_name:b.cloneOf?.name||null,updated_at:iso(Date.now())};
    case 'watch': return {type:b.type,title:b.title,creator:b.creator||'',year:b.year||'',service:b.service||'',rank:b.rank||0,poster_url:b.cover||null,ext_id:b.ext||null};
  }
  throw new Error('Unknown kind '+kind);
}
const kindOfId=id=>{ for(const arr of Object.values(S.items)){ const x=arr.find(i=>i.id===id); if(x) return x.kind; } return null; };
async function put(id,body){
  const kind=body.kind, table=TABLE[kind], row=toRow(kind,body);
  if(id){ const {error}=await sb.from(table).update(row).eq('id',id).eq('user_id',S.uid); if(error) throw error; await loadAll(); return id; }
  const {data,error}=await sb.from(table).insert({...row,user_id:S.uid}).select('id').single(); if(error) throw error;
  await loadAll(); return data.id;
}
async function del(id){ const kind=kindOfId(id); if(!kind) return; const {error}=await sb.from(TABLE[kind]).delete().eq('id',id); if(error) throw error; await loadAll(); }
async function setLike(entryId,on){
  const {error} = on ? await sb.from('likes').insert({entry_id:entryId,user_id:S.uid}) : await sb.from('likes').delete().eq('entry_id',entryId).eq('user_id',S.uid);
  if(error && error.code!=='23505') throw error; await loadAll();
}
async function setFollow(uid,on){
  const {error} = on ? await sb.from('follows').insert({follower:S.uid,followee:uid}) : await sb.from('follows').delete().eq('follower',S.uid).eq('followee',uid);
  if(error && error.code!=='23505') throw error; await loadAll();
}
const PROF_COLS={handle:'handle',display:'display',bio:'bio',location:'location',link:'link',avatar:'avatar_url',backdrop:'banner_url',favs:'favs',pins:'pins',precise:'precise'};
PROF_COLS.display='display_name';
let profQ=Promise.resolve();
function saveProf(patch){
  const run=profQ.catch(()=>{}).then(async()=>{
    const row={}; for(const [k,v] of Object.entries(patch)) if(PROF_COLS[k]) row[PROF_COLS[k]]=v;
    const {error}=await sb.from('profiles').update(row).eq('id',S.uid); if(error) throw error;
    S.prof[S.uid]={...prof(S.uid),...patch};
  });
  profQ=run;
  return run.catch(e=>{ handleErr(e); throw e; });
}
function friendly(e){
  if(!e) return 'Something went wrong.';
  if(e.code==='23505') return 'That username is already taken.';
  if(e.code==='23514') return 'One of the fields is too long or not allowed.';
  if(e.code==='42501') return 'You can only change your own things.';
  return e.message || 'Could not save. Check your connection.';
}
function handleErr(e){ console.error(e); toast(friendly(e)); }
async function shrink(file,max=900){
  return new Promise((res,rej)=>{ const url=URL.createObjectURL(file); const img=new Image();
    img.onload=()=>{ const sc=Math.min(1,max/Math.max(img.width,img.height)); const c=document.createElement('canvas'); c.width=Math.round(img.width*sc); c.height=Math.round(img.height*sc);
      c.getContext('2d').drawImage(img,0,0,c.width,c.height); URL.revokeObjectURL(url); c.toBlob(b=>b?res(b):rej(new Error('That image could not be processed.')),'image/jpeg',0.86); };
    img.onerror=()=>{ URL.revokeObjectURL(url); rej(new Error('That file is not an image this browser can read.')); }; img.src=url; });
}
async function upload(file,max){
  const b=await shrink(file,max); const path=`${S.uid}/${crypto.randomUUID()}.jpg`;
  const {error}=await sb.storage.from('media').upload(path,b,{contentType:'image/jpeg',cacheControl:'31536000'}); if(error) throw error;
  return sb.storage.from('media').getPublicUrl(path).data.publicUrl;
}

/* ---------- Loading ---------- */
async function fetchAll(table,order){
  let out=[], from=0;
  for(;;){ const {data,error}=await sb.from(table).select('*').order(order,{ascending:false}).range(from,from+999); if(error) throw error;
    out=out.concat(data); if(data.length<1000 || out.length>=20000) break; from+=1000; }
  return out;
}
let loading=null, loadT, loadAgain=false;
function scheduleLoad(){ clearTimeout(loadT); loadT=setTimeout(()=>loadAll(),300); }
async function loadAll(){
  if(!sb || !S.uid) return;
  if(loading){ loadAgain=true; return loading; }
  loading=(async()=>{
    try{
      const [profiles,entries,comments,likesR,listsR,watchR,follows]=await Promise.all([
        fetchAll('profiles','created_at'),fetchAll('entries','created_at'),fetchAll('comments','created_at'),fetchAll('likes','created_at'),
        fetchAll('lists','updated_at'),fetchAll('watchlist','created_at'),fetchAll('follows','created_at')]);
      const P={}; profiles.forEach(p=>P[p.id]={handle:p.handle||'',display:p.display_name||'',bio:p.bio||'',location:p.location||'',link:p.link||'',avatar:p.avatar_url,backdrop:p.banner_url,favs:p.favs||{},pins:p.pins||[],precise:!!p.precise,joined:Date.parse(p.created_at)});
      const items={}; const add=(u,x)=>(items[u]=items[u]||[]).push(x); const owner={};
      entries.forEach(r=>{ owner[r.id]=r.user_id; add(r.user_id,{kind:'entry',id:r.id,uid:r.user_id,type:r.type,title:r.title,creator:r.creator,year:r.year,date:r.date,rating:+r.rating||0,liked:r.liked,rewatch:r.rewatch,owned:r.owned,spoilers:r.spoilers,service:r.service,tags:r.tags||[],review:r.review,cover:r.poster_url,backdrop:r.backdrop_url,ext:r.ext_id,createdAt:Date.parse(r.created_at),updatedAt:Date.parse(r.updated_at)}); });
      comments.forEach(r=>{ if(owner[r.entry_id]) add(r.user_id,{kind:'comment',id:r.id,uid:r.user_id,entry:r.entry_id,to:owner[r.entry_id],text:r.text,createdAt:Date.parse(r.created_at),editedAt:r.edited_at?Date.parse(r.edited_at):null}); });
      likesR.forEach(r=>{ if(owner[r.entry_id]) add(r.user_id,{kind:'like',id:r.entry_id+':'+r.user_id,uid:r.user_id,entry:r.entry_id,to:owner[r.entry_id],createdAt:Date.parse(r.created_at)}); });
      listsR.forEach(r=>add(r.user_id,{kind:'list',id:r.id,uid:r.user_id,name:r.name,desc:r.description,ranked:r.ranked,items:r.items||[],cloneOf:r.clone_of_name?{u:r.clone_of_user,i:r.clone_of,name:r.clone_of_name}:null,createdAt:Date.parse(r.created_at),updatedAt:Date.parse(r.updated_at)}));
      watchR.forEach(r=>add(r.user_id,{kind:'watch',id:r.id,uid:r.user_id,type:r.type,title:r.title,creator:r.creator,year:r.year,service:r.service,rank:r.rank,cover:r.poster_url,ext:r.ext_id,createdAt:Date.parse(r.created_at)}));
      S.prof=P; S.items=items; S.follows=follows; S.ready=true;
    }catch(e){ console.error(e); toast('Could not load: '+(e.message||'network error')); }
  })();
  await loading; loading=null; scheduleRender();
  if(loadAgain){ loadAgain=false; scheduleLoad(); }
}

/* ---------- Follows ---------- */
const following=()=>new Set((S.follows||[]).filter(f=>f.follower===S.uid).map(f=>f.followee));
const followerCount=uid=>(S.follows||[]).filter(f=>f.followee===uid).length;
const followingCount=uid=>(S.follows||[]).filter(f=>f.follower===uid).length;
const isFollowing=uid=>following().has(uid);
function inCircle(uid){ const f=following(); return !f.size || uid===S.uid || f.has(uid); }

/* ---------- Catalog (TMDB, Open Library, iTunes via /api) ---------- */
S.catalog=new Map();
function addCatalog(arr){ (arr||[]).forEach(x=>{ if(!x||!x.title) return; x.cover=x.cover||x.poster||null; const k=tkey(x); if(!S.catalog.has(k)) S.catalog.set(k,x); }); }
async function api(path){ const r=await fetch(path); const j=await r.json().catch(()=>({})); if(!r.ok) throw new Error(j.error||('Request failed: '+r.status)); return j; }
const detailCache=new Map();
function fetchDetails(ext){ if(!ext) return Promise.resolve(null); if(!detailCache.has(ext)) detailCache.set(ext, api('/api/details?ext='+encodeURIComponent(ext)).catch(()=>null)); return detailCache.get(ext); }
async function loadExplore(){
  try{ const r=await api('/api/explore');
    (r.sections||[]).forEach(s=>(s.items||[]).forEach(x=>x.cover=x.poster)); if(r.spotlight) r.spotlight.cover=r.spotlight.poster;
    (r.sections||[]).forEach(s=>addCatalog(s.items)); if(r.spotlight) addCatalog([r.spotlight]);
    S.explore=r; S.exploreErr=null;
  }catch(e){ S.exploreErr=e.message; }
  scheduleRender();
}
async function remoteSearch(){
  const q=S.q.trim(); if(q.length<2){ S.remote=null; return; }
  const key=q+'|'+S.qType; if(S.remote && S.remote.key===key) return;
  S.remote={key,loading:true,results:[]}; softRender();
  try{ const r=await api(`/api/search?type=${encodeURIComponent(S.qType)}&q=${encodeURIComponent(q)}`);
    if(!S.remote || S.remote.key!==key) return;
    (r.results||[]).forEach(x=>x.cover=x.poster); addCatalog(r.results);
    S.remote={key,results:r.results||[],tmdb:r.tmdb};
  }catch(e){ if(S.remote && S.remote.key===key) S.remote={key,results:[],err:e.message}; }
  if(cur().v==='search') softRender();
}

/* ---------- Click handling ---------- */
document.addEventListener('click', async ev=>{
  const el=ev.target.closest('[data-a]'); if(!el) return;
  if(el.closest('#sheetRoot') && !el.closest('.sheet')) return;
  const a=el.dataset.a, d=el.dataset, r=cur();
  switch(a){
    case 'nav': if(d.k==='search') S.focusQ=true; tab(d.k==='profile'?{v:'profile',u:S.uid,t:'profile'}:{v:d.k}); break;
    case 'back': back(); break;
    case 'entry': go({v:'entry',u:d.u,i:d.i}); break;
    case 'list': go({v:'list',u:d.u,i:d.i}); break;
    case 'title': go({v:'title',k:d.k}); break;
    case 'profile': go({v:'profile',u:d.u,t:'profile'}); break;
    case 'ptab': r.t=d.t; render(); break;
    case 'favtype': S.favType=d.k; render(); break;
    case 'ftype': S[d.key]=d.k; render(); if(d.key==='qType'){ S.remote=null; remoteSearch(); } break;
    case 'fkind': S.fkind=d.k; render(); break;
    case 'htab': S.homeTab=d.k; render(true); break;
    case 'xsection': go({v:'section',n:+d.n}); break;
    case 'log-x': { ev.stopPropagation(); const x=titleMap().get(d.k)||exploreFind(d.k); if(x) openLog(null,{type:x.type,title:x.title,year:x.year||'',creator:x.creator||'',ext:x.ext||null,backdrop:x.backdrop||null,cover:x.cover||coverIndex().get(x.type+'|'+String(x.title).trim().toLowerCase())||null}); break; }
    case 'watch-x': { ev.stopPropagation(); const x=titleMap().get(d.k)||exploreFind(d.k); if(x) await addWatch({type:x.type,title:x.title,year:x.year||'',creator:x.creator||'',cover:x.cover||null,ext:x.ext||null,service:x.service||''}); break; }
    case 'syear': S.statsYear=d.k; render(); break;
    case 'dOwned': S.dOwned=!S.dOwned; render(); break;
    case 'dSvc': S.dSvc = S.dSvc===d.k?'':d.k; render(); break;
    case 'wSvc': S.wSvc = S.wSvc===d.k?'':d.k; render(); break;
    case 'unspoil': el.classList.remove('spoil'); const n=el.nextElementSibling; if(n&&n.classList.contains('spoilnote')) n.remove(); break;
    case 'log': openLog(null); break;
    case 'log-new': openLog(null,{title:d.t}); break;
    case 'log-title': { const t=titleMap().get(d.k); const lg=t&&t.logs.find(l=>l.ext); openLog(null,t?{type:t.type,title:t.title,year:t.year,creator:t.creator,cover:t.cover,ext:lg?lg.ext:null,backdrop:lg?lg.backdrop:null}:{}); break; }
    case 'watch-title': { const t=titleMap().get(d.k); if(t) await addWatch({type:t.type,title:t.title,year:t.year,creator:t.creator,cover:t.cover||null,service:''}); break; }
    case 'edit-entry': openLog(find(r.u,r.i)); break;
    case 'import': openImport(); break;
    case 'edit-profile': openProfile(); break;
    case 'fav-pick': openFavPick(); break;
    case 'fav-entry': { const e=find(r.u,r.i); const f={...(prof(S.uid).favs||{})}; const arr=[...(f[e.type]||[])]; const k=arr.indexOf(e.id);
      if(k>=0) arr.splice(k,1); else { if(arr.length>=4){ toast('You already have 4 favorite '+TYPES[e.type].plural.toLowerCase()+'. Remove one first.'); break; } arr.push(e.id); }
      f[e.type]=arr; await saveProf({favs:f}); render(); toast(k>=0?'Removed from favorites':'Added to favorites'); break; }
    case 'pin-entry': case 'pin-list': { const id=a==='pin-entry'?r.i:d.i; const pins=[...(prof(S.uid).pins||[])]; const k=pins.findIndex(x=>x.id===id);
      if(k>=0) pins.splice(k,1); else { if(pins.length>=3){ toast('You can pin up to 3. Unpin one first.'); break; } pins.unshift({id}); }
      await saveProf({pins}); render(); toast(k>=0?'Unpinned':'Pinned to your profile'); break; }
    case 'like': { try{ await setLike(r.i, !likes(r.u,r.i).some(l=>isMe(l.uid))); }catch(e){ handleErr(e);} break; }
    case 'follow': { el.disabled=true; try{ await setFollow(d.u, !isFollowing(d.u)); }catch(e){ handleErr(e);} break; }
    case 'share-profile': { const url=location.origin+location.pathname+'#@'+nm(d.u); try{ await navigator.clipboard.writeText(url); toast('Profile link copied'); }catch(e){ toast(url); } break; }
    case 'si-reset': S.siSent=null; render(); break;
    case 'noop': break;
    case 'c-post': { const ta=$('#cnew'); const text=ta.value.trim(); if(!text) break; el.disabled=true;
      try{ await put(null,{kind:'comment',to:r.u,entry:r.i,text,createdAt:Date.now()}); ta.value=''; }catch(e){ handleErr(e); } el.disabled=false; break; }
    case 'c-edit': S.cEdit=d.i; S.cDel=null; render(); setTimeout(()=>$('#cedit')?.focus(),0); break;
    case 'c-cancel': S.cEdit=null; render(); break;
    case 'c-save': { const c=find(S.uid,d.i); const text=$('#cedit').value.trim(); if(!text||!c) break; try{ const {id,uid,...rest}=c; await put(d.i,{...rest,text,editedAt:Date.now()}); S.cEdit=null; }catch(e){ handleErr(e);} break; }
    case 'c-del': if(S.cDel!==d.i){ S.cDel=d.i; render(); break; } try{ await del(d.i); S.cDel=null; toast('Comment deleted'); }catch(e){ handleErr(e);} break;
    case 'new-list': openList(null); break;
    case 'edit-list': openList(find(S.uid,d.i)); break;
    case 'clone-list': { const l=find(d.u,d.i); if(!l) break; try{ const id=await put(null,{kind:'list',name:l.name+(isMe(d.u)?' (copy)':''),desc:l.desc||'',ranked:!!l.ranked,items:(l.items||[]).map(x=>({...x})),cloneOf:{u:d.u,i:d.i,name:l.name},createdAt:Date.now(),updatedAt:Date.now()}); toast('Cloned to your lists'); go({v:'list',u:S.uid,i:id}); }catch(e){ handleErr(e);} break; }
    case 'to-list': openToList(d.k); break;
    case 'watch-add': openWatch(); break;
    case 'w-up': case 'w-down': { const ws=watch(S.uid); const k=ws.findIndex(w=>w.id===d.i); const j=a==='w-up'?k-1:k+1; if(j<0||j>=ws.length) break;
      [ws[k],ws[j]]=[ws[j],ws[k]]; try{ for(let n=0;n<ws.length;n++){ if(ws[n].rank!==n+1){ const {id,uid,...rest}=ws[n]; await put(id,{...rest,rank:n+1}); } } }catch(e){ handleErr(e);} break; }
    case 'w-del': { const w=find(S.uid,d.i); try{ await del(d.i); toast('Removed from watchlist'); }catch(e){ handleErr(e);} break; }
    case 'w-log': { const w=find(S.uid,d.i); openLog(null,{type:w.type,title:w.title,year:w.year,creator:w.creator,cover:w.cover,service:w.service,fromWatch:w.id}); break; }
  }
});
document.addEventListener('input', ev=>{ if(ev.target.id==='q'){ S.q=ev.target.value; clearTimeout(S.qT); S.qT=setTimeout(()=>{ softRender(); remoteSearch(); },250); } });

/* ---------- Sheets ---------- */
function sheet(html, wire, opt={}){
  $('#sheetRoot').innerHTML=`<div class="scrim" id="scrim"><div class="sheet" role="dialog" aria-modal="true"><div class="grab"></div>${html}</div></div>`;
  if(opt.locked) $('#scrim').dataset.locked='1'; else $('#scrim').addEventListener('click',e=>{ if(e.target.id==='scrim') closeSheet(); });
  document.querySelectorAll('[data-close]').forEach(b=>b.onclick=closeSheet);
  wire && wire();
}
function closeSheet(){ if(D&&D.bdPreview) URL.revokeObjectURL(D.bdPreview); if(D&&D.preview) URL.revokeObjectURL(D.preview); D=null; $('#sheetRoot').innerHTML=''; }
document.addEventListener('keydown',e=>{ if(e.key==='Escape' && $('#scrim') && !$('#scrim').dataset.locked) closeSheet(); });

/* --- Log / edit entry --- */
let D=null;
function loadDraft(){ try{ return JSON.parse(localStorage.getItem('artistry-draft')||'null'); }catch(e){ return null; } }
function saveDraft(){ if(!D||D.id) return; try{ const {file,preview,bdFile,bdPreview,...x}=D; localStorage.setItem('artistry-draft',JSON.stringify(x)); }catch(e){} }
function clearDraft(){ try{ localStorage.removeItem('artistry-draft'); }catch(e){} }
function openLog(entry,pre){
  const base={id:null,ext:null,backdrop:null,type:'film',title:'',creator:'',year:'',date:today(),rating:0,liked:false,rewatch:false,owned:false,spoilers:false,service:'',tags:'',review:'',cover:null};
  let restored=false;
  if(entry){ const {uid,kind,...rest}=entry; D={...base,...rest,tags:(entry.tags||[]).join(', ')}; }
  else if(pre){ D={...base,...pre,pickedTitle:pre.title}; if(ents(S.uid).some(e=>tkey(e)===tkey(D))) D.rewatch=true;
    if(D.ext && !D.creator && /^tmdb:/.test(D.ext)){ const want=D.ext; fetchDetails(want).then(det=>{ if(det&&det.creator&&D&&D.ext===want&&!D.creator){ D.creator=det.creator; const f=$('#f-creator'); if(f) f.value=det.creator; } }); } }
  else { const dr=loadDraft(); if(dr && (dr.title||dr.review)){ D={...base,...dr}; restored=true; } else D={...base}; }
  D.file=null; D.preview=null; D.bdFile=null; D.bdPreview=null; D.confirmDel=false; D.restored=restored;
  drawLog();
}
function drawLog(err){
  const t=TYPES[D.type]; const precise=!!prof(S.uid).precise;
  const cov = D.preview?`<div class="cv" data-type="${D.type}"><img src="${D.preview}" alt=""></div>`:cover({...D,title:D.title||t.label});
  sheet(`<div class="sh"><h2>${D.id?'Edit entry':'Log'}</h2><button class="linkbtn" data-close style="color:var(--muted)">Cancel</button></div>
    ${D.restored?`<div class="hint">Restored your unsaved draft. <button class="linkbtn" id="f-discard">Discard</button></div>`:''}
    <div class="seg" role="radiogroup" aria-label="Type">${TK.map(k=>`<button role="radio" data-type="${k}" data-settype="${k}" aria-checked="${D.type===k}">${I[k]}${TYPES[k].label}</button>`).join('')}</div>
    <div class="field"><label for="f-title">Title</label><input id="f-title" maxlength="160" autocomplete="off" value="${esc(D.title)}" placeholder="Search or type a title, ${t.ph.replace('e.g. ','like ')}"><div id="sugg" class="sugg"></div>${D.ext?`<span class="hint matched">${I.eye} Matched to ${esc(D.ext.startsWith('tmdb')?'TMDB':D.ext.startsWith('ol')?'Open Library':'Apple Music')}</span>`:''}</div>
    <div class="row2"><div class="field"><label for="f-creator">${t.creator}</label><input id="f-creator" maxlength="120" value="${esc(D.creator)}"></div>
      <div class="field"><label for="f-year">Year</label><input id="f-year" inputmode="numeric" maxlength="4" value="${esc(D.year)}" placeholder="2024"></div></div>
    <div class="field"><span class="flabel">Rating ${precise?'(precise)':''}</span>
      <div class="ratebar"><div class="starpick" id="starpick" style="--p:${D.rating*20}%">★★★★★<input id="f-rating" type="range" min="0" max="5" step="${precise?0.1:0.5}" value="${D.rating}" aria-label="Rating out of 5"></div><span class="rateval mono" id="rateval">${D.rating?(precise?D.rating.toFixed(1):stars(D.rating)):'None'}</span></div>
      <span class="hint">Tap or drag across the stars. Tap the same spot again to clear.${precise?'':' Want 0.1 steps? Turn on precise ratings in profile settings.'}</span></div>
    <div class="toggles"><button class="tog h" id="f-like" aria-pressed="${D.liked}">${I.heart}Loved</button><button class="tog" id="f-rew" aria-pressed="${D.rewatch}">${I.rew}${D.type==='book'?'Reread':D.type==='music'?'Relisten':'Rewatch'}</button><button class="tog" id="f-own" aria-pressed="${D.owned}">${I.disc}In my collection</button></div>
    <div class="row2"><div class="field"><label for="f-date">Finished on</label><input id="f-date" type="date" value="${esc(D.date)}" max="${today()}"></div>
      <div class="field"><label for="f-svc">Where</label><input id="f-svc" list="svcs" maxlength="40" value="${esc(D.service)}" placeholder="Netflix, Theater…"></div></div>
    <datalist id="svcs">${SERVICES.map(s=>`<option value="${esc(s)}">`).join('')}</datalist>
    <div class="field"><label for="f-review">Review</label><textarea id="f-review" maxlength="5000" placeholder="Optional">${esc(D.review)}</textarea>
      <label class="chk"><input type="checkbox" id="f-spoil" ${D.spoilers?'checked':''}> Contains spoilers</label></div>
    <div class="field"><label for="f-tags">Tags</label><input id="f-tags" maxlength="200" value="${esc(D.tags)}" placeholder="comfort, a24, road trip"></div>
    ${S.assets?`<div class="field"><span class="flabel">Poster or cover</span><div class="coverpick">${cov}<div class="c"><label class="btn sm" for="f-file">${D.cover||D.preview?'Change image':'Choose image'}</label><input id="f-file" type="file" accept="image/*" hidden>${D.cover||D.preview?'<button class="linkbtn" id="f-nocov">Use generated cover</button>':''}${D.ext&&!/^itunes:/.test(D.ext)?'<button class="linkbtn" id="f-pickpos">Choose from official posters</button>':''}<span class="hint">Upload any poster or edition you like. It shows on your log, lists and favorites.</span></div></div></div>`:''}
    ${S.assets?`<div class="field"><span class="flabel">Backdrop (optional)</span><div class="coverpick"><div style="width:96px;aspect-ratio:16/9;border-radius:6px;background:var(--raise) center/cover;${D.bdPreview?`background-image:url('${D.bdPreview}')`:(D.backdrop?`background-image:url('${blob(esc(D.backdrop))}')`:'')}"></div><div class="c"><label class="btn sm" for="f-bd">${D.backdrop||D.bdPreview?'Change backdrop':'Add a backdrop'}</label><input id="f-bd" type="file" accept="image/*" hidden>${D.backdrop||D.bdPreview?'<button class="linkbtn" id="f-nobd">Remove</button>':''}${D.ext&&/^tmdb:/.test(D.ext)?'<button class="linkbtn" id="f-pickbd">Choose a scene still</button>':''}<span class="hint">A wide scene still or photo. Shows as the banner on this entry.</span></div></div></div>`:''}
    ${err?`<div class="err">${esc(err)}</div>`:''}
    <div class="actions">${D.id?`<button class="del" id="f-del">${D.confirmDel?'Tap again to delete':'Delete entry'}</button>`:'<span></span>'}<button class="btn pri" id="f-save">${D.id?'Save changes':'Save'}</button></div>`, wireLog);
}
function pullLog(){ if(!D) return; D.title=$('#f-title').value; D.creator=$('#f-creator').value; D.year=$('#f-year').value.replace(/\D/g,''); D.date=$('#f-date').value||today(); D.review=$('#f-review').value; D.service=$('#f-svc').value.trim(); D.tags=$('#f-tags').value; D.spoilers=$('#f-spoil').checked; }
function setRating(v){ const precise=!!prof(S.uid).precise; D.rating=Math.round(v*10)/10; $('#starpick').style.setProperty('--p',D.rating*20+'%'); $('#f-rating').value=D.rating; $('#rateval').textContent=D.rating?(precise?D.rating.toFixed(1):stars(D.rating)):'None'; saveDraft(); }
function wireLog(){
  const precise=!!prof(S.uid).precise;
  document.querySelectorAll('[data-settype]').forEach(b=>b.onclick=()=>{ pullLog(); D.type=b.dataset.settype; drawLog(); });
  ['f-like:liked','f-rew:rewatch','f-own:owned'].forEach(x=>{ const [id,k]=x.split(':'); $('#'+id).onclick=()=>{ D[k]=!D[k]; $('#'+id).setAttribute('aria-pressed',String(D[k])); saveDraft(); }; });
  const sp=$('#starpick'); let down=false,start=0,moved=false,sx=0;
  const val=x=>{ const r=sp.getBoundingClientRect(); const f=Math.max(0,Math.min(1,(x-r.left)/r.width)); return precise? Math.max(0.1,Math.round(f*50)/10) : Math.max(0.5,Math.ceil(f*10)/2); };
  sp.addEventListener('pointerdown',e=>{ down=true;moved=false;start=D.rating;sx=e.clientX; sp.setPointerCapture(e.pointerId); setRating(val(e.clientX)); });
  sp.addEventListener('pointermove',e=>{ if(!down) return; if(Math.abs(e.clientX-sx)>4) moved=true; setRating(val(e.clientX)); });
  sp.addEventListener('pointerup',e=>{ if(!down) return; down=false; if(!moved && val(e.clientX)===start) setRating(0); });
  $('#f-rating').addEventListener('input',e=>setRating(+e.target.value));
  $('.sheet').addEventListener('input',()=>{ pullLog(); saveDraft(); });
  const dis=$('#f-discard'); if(dis) dis.onclick=()=>{ clearDraft(); openLog(null); };
  const f=$('#f-file'); if(f) f.onchange=()=>{ const file=f.files[0]; if(!file) return; pullLog(); if(D.preview) URL.revokeObjectURL(D.preview); D.file=file; D.preview=URL.createObjectURL(file); drawLog(); };
  const fb=$('#f-bd'); if(fb) fb.onchange=()=>{ const file=fb.files[0]; if(!file) return; pullLog(); if(D.bdPreview) URL.revokeObjectURL(D.bdPreview); D.bdFile=file; D.bdPreview=URL.createObjectURL(file); drawLog(); };
  const nb=$('#f-nobd'); if(nb) nb.onclick=()=>{ pullLog(); if(D.bdPreview) URL.revokeObjectURL(D.bdPreview); D.bdFile=null; D.bdPreview=null; D.backdrop=null; drawLog(); };
  const nc=$('#f-nocov'); if(nc) nc.onclick=()=>{ pullLog(); if(D.preview) URL.revokeObjectURL(D.preview); D.file=null; D.preview=null; D.cover=null; drawLog(); };
  const dl=$('#f-del'); if(dl) dl.onclick=async()=>{ pullLog(); if(!D.confirmDel){ D.confirmDel=true; dl.textContent='Tap again to delete'; return; }
    try{ const id=D.id; await del(id); const p=prof(S.uid); const favs={...(p.favs||{})}; let ch=false; TK.forEach(k=>{ if((favs[k]||[]).includes(id)){ favs[k]=favs[k].filter(x=>x!==id); ch=true; } }); const pins=(p.pins||[]).filter(x=>x.id!==id);
      if(ch||pins.length!==(p.pins||[]).length) await saveProf({favs,pins}); closeSheet(); toast('Deleted'); if(cur().v==='entry') back(); }catch(e){ drawLog('Could not delete. Try again.'); } };
  $('#f-save').onclick=saveLog;
  const pp=$('#f-pickpos'); if(pp) pp.onclick=()=>{ pullLog(); openImgPicker('posters'); };
  const pb=$('#f-pickbd'); if(pb) pb.onclick=()=>{ pullLog(); openImgPicker('backdrops'); };
  const ti=$('#f-title'); let sT, seq=0;
  ti.addEventListener('input',()=>{ if(D.ext && ti.value.trim()!==D.pickedTitle){ D.ext=null; const m=document.querySelector('.matched'); if(m) m.remove(); }
    clearTimeout(sT); sT=setTimeout(async()=>{ const q=ti.value.trim(); const box=$('#sugg'); if(!box) return; if(q.length<2||D.ext){ box.innerHTML=''; return; }
      const my=++seq; box.innerHTML='<div class="hint" style="padding:8px 2px">Searching…</div>';
      try{ const r=await api(`/api/search?type=${D.type}&q=${encodeURIComponent(q)}`); if(my!==seq||!$('#sugg')) return;
        const res=(r.results||[]).slice(0,6); res.forEach(x=>x.cover=x.poster);
        $('#sugg').innerHTML=res.map((x,n)=>`<button class="sg" data-sg="${n}" data-type="${x.type}">${cover(x)}<span><b>${esc(x.title)}</b><small>${esc([x.year,x.creator].filter(Boolean).join(' · '))}</small></span></button>`).join('')||'<div class="hint" style="padding:8px 2px">No matches. You can still log it with your own title.</div>';
        $('#sugg').querySelectorAll('[data-sg]').forEach(b=>b.onclick=async()=>{ const x=res[+b.dataset.sg]; pullLog();
          Object.assign(D,{title:x.title,year:x.year||D.year,creator:x.creator||D.creator,cover:x.poster||D.cover,ext:x.ext,pickedTitle:x.title}); if(!D.backdrop&&x.backdrop) D.backdrop=x.backdrop;
          if(D.preview){ URL.revokeObjectURL(D.preview); D.preview=null; D.file=null; }
          addCatalog([x]); drawLog(); saveDraft();
          if(!x.creator && /^tmdb:/.test(x.ext||'')){ const det=await fetchDetails(x.ext); if(det&&det.creator&&D&&D.ext===x.ext&&!D.creator){ D.creator=det.creator; const f=$('#f-creator'); if(f) f.value=det.creator; saveDraft(); } } });
      }catch(e){ if($('#sugg')) $('#sugg').innerHTML=`<div class="hint" style="padding:8px 2px">${esc(e.message)}</div>`; } },300); });
  if(!D.id) setTimeout(()=>{ if(!D.title) $('#f-title')?.focus(); },60);
}
async function saveLog(){
  pullLog();
  if(!D.title.trim()){ drawLog('Add a title first.'); return; }
  const b=$('#f-save'); b.disabled=true; b.textContent='Saving…';
  const old=D.id?find(S.uid,D.id):null;
  try{
    let cov=D.cover||null; if(D.file) cov=await upload(D.file);
    let bdrop=D.backdrop||null; if(D.bdFile) bdrop=await upload(D.bdFile,1600);
    const body={kind:'entry',type:D.type,title:D.title.trim(),creator:D.creator.trim(),year:D.year,date:D.date,rating:D.rating||0,liked:!!D.liked,rewatch:!!D.rewatch,owned:!!D.owned,
      spoilers:!!D.spoilers,service:D.service,ext:D.ext||null,tags:D.tags.split(',').map(s=>s.trim().replace(/^#/,'')).filter(Boolean).slice(0,12),review:D.review.trim(),cover:cov,backdrop:bdrop,createdAt:old?.createdAt||Date.now(),updatedAt:Date.now()};
    const id=await put(D.id,body);
    if(D.fromWatch){ try{ await del(D.fromWatch); }catch(e){} }
    const wasEdit=!!D.id; if(!wasEdit) clearDraft(); closeSheet(); toast(wasEdit?'Saved':'Logged');
    if(!wasEdit) go({v:'entry',u:S.uid,i:id});
  }catch(e){
    drawLog(friendly(e));
  }
}

/* --- Official poster / backdrop picker --- */
async function openImgPicker(kind){
  const back=()=>drawLog();
  sheet(`<div class="sh"><h2>${kind==='posters'?'Choose a poster':'Choose a scene still'}</h2><button class="linkbtn" id="ip-back" style="color:var(--muted)">Back</button></div><div id="ip-body" class="hint">Loading images…</div>`, ()=>{ $('#ip-back').onclick=back; });
  const d=await fetchDetails(D.ext); const list=(d&&d[kind])||[]; const box=$('#ip-body'); if(!box) return;
  if(!list.length){ box.textContent='No images found for this title.'; return; }
  box.className=kind==='posters'?'pickgrid':'bdpick';
  box.innerHTML=list.map((u,n)=>kind==='posters'
    ? `<button data-ip="${n}" aria-pressed="${D.cover===u}"><div class="cv" data-type="${D.type}"><img src="${esc(u)}" alt="" loading="lazy"></div></button>`
    : `<button data-ip="${n}" aria-pressed="${D.backdrop===u}" style="background-image:url('${esc(u)}')" aria-label="Use this still"></button>`).join('');
  box.querySelectorAll('[data-ip]').forEach(b=>b.onclick=()=>{ const u=list[+b.dataset.ip]; if(kind==='posters'){ D.cover=u; if(D.preview){ URL.revokeObjectURL(D.preview); D.preview=null; D.file=null; } } else { D.backdrop=u; if(D.bdPreview){ URL.revokeObjectURL(D.bdPreview); D.bdPreview=null; D.bdFile=null; } } saveDraft(); back(); });
}

/* --- Profile settings --- */
function openProfile(){
  const p=prof(S.uid); let av=null, bd=null, bdId=p.backdrop||null;
  const bdChoices=[...new Set(ents(S.uid).flatMap(e=>[e.backdrop,e.cover]).filter(Boolean))].slice(0,12);
  sheet(`<div class="sh"><h2>Profile</h2><button class="linkbtn" data-close style="color:var(--muted)">Close</button></div>
    <div class="row2"><div class="field"><label for="p-handle">Username</label><input id="p-handle" maxlength="30" autocapitalize="none" value="${esc(p.handle||'')}"></div><div class="field"><label for="p-name">Name</label><input id="p-name" maxlength="50" value="${esc(p.display||'')}"></div></div>
    <span class="hint">Change either one whenever you like.</span>
    <div class="field"><label for="p-bio">Bio</label><textarea id="p-bio" maxlength="240" style="min-height:70px">${esc(p.bio||'')}</textarea></div>
    <div class="row2"><div class="field"><label for="p-loc">Location</label><input id="p-loc" maxlength="40" value="${esc(p.location||'')}"></div><div class="field"><label for="p-link">Website</label><input id="p-link" maxlength="120" inputmode="url" value="${esc(p.link||'')}" placeholder="yoursite.com"></div></div>
    ${S.assets?`<div class="row2"><div class="field"><span class="flabel">Profile photo</span><label class="btn sm" for="p-av" style="align-self:flex-start">Choose photo</label><input id="p-av" type="file" accept="image/*" hidden><span class="hint" id="p-avn">${p.avatar?'Custom photo set':'Using your account photo'}</span></div>
      <div class="field"><span class="flabel">Backdrop</span><label class="btn sm" for="p-bd" style="align-self:flex-start">Choose image</label><input id="p-bd" type="file" accept="image/*" hidden><span class="hint" id="p-bdn">${p.backdrop?'Banner set':'Upload a wide image, or pick one below'}</span></div></div>
    <div class="field"><label for="p-film">Or pick a still from any film or show</label><input id="p-film" autocomplete="off" placeholder="Search films and shows"><div id="p-films" class="sugg"></div><div id="p-stills" class="bdpick"></div></div>
    ${bdChoices.length?`<div class="field"><span class="flabel">Or use an image from your logs</span><div class="bdpick">${p.backdrop?`<button data-bdid="" aria-label="No banner" style="display:grid;place-items:center;color:var(--muted);font-size:12px;font-weight:600">None</button>`:''}${bdChoices.map(id=>`<button data-bdid="${esc(id)}" aria-pressed="${p.backdrop===id}" style="background-image:url('${esc(blob(id))}')" aria-label="Use this image"></button>`).join('')}</div></div>`:''}`:''}
    <label class="chk"><input type="checkbox" id="p-precise" ${p.precise?'checked':''}> <span>Precise ratings<br><span class="hint">Rate in 0.1 steps (like 4.3) instead of half stars.</span></span></label>
    <div class="actions"><span></span><button class="btn pri" id="p-save">Save profile</button></div>
    <div class="sec"><h2>Bring your history</h2></div>
    <button class="optrow" id="p-import"><span><b>Import from Letterboxd</b><br><span class="hint">Diary, ratings, reviews, likes and watchlist from your export.</span></span>${I.upload.replace('<svg','<svg width="20" height="20"')}</button>
    <button class="optrow" id="p-fill"><span><b>Find missing covers</b><br><span class="hint" id="p-fillst">Looks up posters and album art for entries that don’t have one.</span></span>${I.search.replace('<svg','<svg width="20" height="20"')}</button>
    <div class="sec"><h2>Account</h2></div>
    <button class="optrow" id="p-out"><span><b>Sign out</b></span></button>
    <p class="foot">Film and TV data and images from TMDB. This product uses the TMDB API but is not endorsed or certified by TMDB. Book covers from Open Library. Album art from Apple Music.</p>`, ()=>{
    const pa=$('#p-av'); if(pa) pa.onchange=()=>{ av=pa.files[0]; $('#p-avn').textContent=av?av.name:''; };
    const pb=$('#p-bd'); if(pb) pb.onchange=()=>{ bd=pb.files[0]; $('#p-bdn').textContent=bd?bd.name:''; document.querySelectorAll('[data-bdid]').forEach(x=>x.setAttribute('aria-pressed','false')); };
    document.querySelectorAll('[data-bdid]').forEach(b=>b.onclick=()=>{ bd=null; bdId=b.dataset.bdid||null; document.querySelectorAll('[data-bdid]').forEach(x=>x.setAttribute('aria-pressed',String(x===b))); $('#p-bdn').textContent=bdId?'Using an image from your logs':'No banner'; });
    $('#p-import').onclick=openImport;
    $('#p-out').onclick=async()=>{ await sb.auth.signOut(); closeSheet(); };
    $('#p-fill').onclick=async()=>{ const b=$('#p-fill'); b.disabled=true; await fillPosters($('#p-fillst')); b.disabled=false; };
    $('#p-handle').oninput=e=>{ e.target.value=e.target.value.toLowerCase().replace(/[^a-z0-9_.]/g,''); };
    let fT; $('#p-film').oninput=e=>{ clearTimeout(fT); fT=setTimeout(async()=>{ const q=e.target.value.trim(); const box=$('#p-films'); if(q.length<2){ box.innerHTML=''; return; }
      try{ const r=await api('/api/search?type=all&q='+encodeURIComponent(q)); const res=(r.results||[]).filter(x=>/^tmdb:/.test(x.ext||'')).slice(0,5); res.forEach(x=>x.cover=x.poster);
        box.innerHTML=res.map((x,n)=>`<button class="sg" data-pf="${n}" data-type="${x.type}">${cover(x)}<span><b>${esc(x.title)}</b><small>${esc(x.year||'')}</small></span></button>`).join('')||'<div class="hint">No films or shows found.</div>';
        box.querySelectorAll('[data-pf]').forEach(b=>b.onclick=async()=>{ const x=res[+b.dataset.pf]; box.innerHTML=''; const st=$('#p-stills'); st.innerHTML='<span class="hint">Loading stills…</span>';
          const det=await fetchDetails(x.ext); const list=(det&&det.backdrops)||[]; if(!list.length){ st.innerHTML='<span class="hint">No stills for this title.</span>'; return; }
          st.innerHTML=list.map((u,n)=>`<button data-st="${n}" style="background-image:url('${esc(u)}')" aria-label="Use this still"></button>`).join('');
          st.querySelectorAll('[data-st]').forEach(sb2=>sb2.onclick=()=>{ bd=null; bdId=list[+sb2.dataset.st]; document.querySelectorAll('[data-bdid],[data-st]').forEach(z=>z.setAttribute('aria-pressed',String(z===sb2))); $('#p-bdn').textContent='Using a still from '+x.title; }); });
      }catch(err){ box.innerHTML=`<div class="hint">${esc(err.message)}</div>`; } },300); };
    $('#p-save').onclick=async()=>{ const b=$('#p-save'); b.disabled=true; b.textContent='Saving…';
      try{ const h=$('#p-handle').value.trim(); if(!/^[a-z0-9_.]{2,30}$/.test(h)) throw new Error('Usernames are 2–30 lowercase letters, numbers, dots or underscores.');
        const patch={handle:h,display:$('#p-name').value.trim(),bio:$('#p-bio').value.trim(),location:$('#p-loc').value.trim(),link:safeUrl($('#p-link').value)?$('#p-link').value.trim():'',precise:$('#p-precise').checked,backdrop:bdId};
        if(av) patch.avatar=await upload(av,400); if(bd) patch.backdrop=await upload(bd,1600);
        await saveProf(patch); closeSheet(); toast('Profile saved'); render(); }catch(e){ b.disabled=false; b.textContent='Save profile'; toast(friendly(e)); } };
  });
}

/* --- Favorites picker --- */
function openFavPick(){
  const ft=S.favType; const es=ents(S.uid).filter(e=>e.type===ft).sort((a,b)=>(b.rating||0)-(a.rating||0));
  const sel=new Set(((prof(S.uid).favs||{})[ft])||[]);
  const draw=()=>sheet(`<div class="sh"><h2>Favorite ${TYPES[ft].plural.toLowerCase()}</h2><button class="btn pri sm" id="fv-save">Done</button></div><p class="hint">Pick up to 4 from your diary. ${sel.size}/4 chosen.</p>
    ${es.length?`<div class="pickgrid">${es.map(e=>`<button data-pick="${esc(e.id)}" aria-pressed="${sel.has(e.id)}">${cover(e)}<span style="display:block;margin-top:4px;overflow:hidden;white-space:nowrap;text-overflow:ellipsis">${esc(e.title)}</span></button>`).join('')}</div>`:`<p class="ghost">Log some ${TYPES[ft].plural.toLowerCase()} first.</p>`}`, ()=>{
      document.querySelectorAll('[data-pick]').forEach(b=>b.onclick=()=>{ const id=b.dataset.pick; if(sel.has(id)) sel.delete(id); else if(sel.size<4) sel.add(id); else { toast('Up to 4'); return; } draw(); });
      $('#fv-save').onclick=async()=>{ const favs={...(prof(S.uid).favs||{})}; favs[ft]=[...sel]; await saveProf({favs}); closeSheet(); render(); };
    });
  draw();
}

/* --- Watchlist add --- */
async function addWatch(w){
  if(watch(S.uid).some(x=>tkey(x)===tkey(w))){ toast('Already on your watchlist'); return; }
  try{ const rank=Math.max(0,...watch(S.uid).map(x=>x.rank||0))+1; await put(null,{kind:'watch',...w,rank,createdAt:Date.now()}); toast('Added to watchlist'); }catch(e){ handleErr(e); }
}
function openWatch(){
  let type='film';
  const draw=()=>sheet(`<div class="sh"><h2>Add to watchlist</h2><button class="linkbtn" data-close style="color:var(--muted)">Cancel</button></div>
    <div class="seg">${TK.map(k=>`<button role="radio" data-type="${k}" data-wt="${k}" aria-checked="${type===k}">${I[k]}${TYPES[k].label}</button>`).join('')}</div>
    <div class="field"><label for="w-title">Title</label><input id="w-title" maxlength="160" placeholder="${TYPES[type].ph}"></div>
    <div class="row2"><div class="field"><label for="w-cr">${TYPES[type].creator}</label><input id="w-cr" maxlength="120"></div><div class="field"><label for="w-yr">Year</label><input id="w-yr" inputmode="numeric" maxlength="4"></div></div>
    <div class="field"><label for="w-svc">Available on</label><input id="w-svc" list="svcs2" maxlength="40" placeholder="Where you can get it"><datalist id="svcs2">${SERVICES.map(s=>`<option value="${esc(s)}">`).join('')}</datalist></div>
    <div class="actions"><span></span><button class="btn pri" id="w-save">Add</button></div>`, ()=>{
      document.querySelectorAll('[data-wt]').forEach(b=>b.onclick=()=>{ type=b.dataset.wt; draw(); });
      $('#w-title').focus();
      $('#w-save').onclick=async()=>{ const title=$('#w-title').value.trim(); if(!title){ $('#w-title').focus(); return; }
        await addWatch({type,title,creator:$('#w-cr').value.trim(),year:$('#w-yr').value.replace(/\D/g,''),service:$('#w-svc').value.trim(),cover:null}); closeSheet(); };
    });
  draw();
}

/* --- Lists --- */
function openList(l,first){
  const L = l ? {id:l.id,name:l.name,desc:l.desc||'',ranked:!!l.ranked,items:(l.items||[]).map(x=>({...x})),createdAt:l.createdAt,cloneOf:l.cloneOf||null}
              : {id:null,name:'',desc:'',ranked:false,items:first?[first]:[],createdAt:Date.now(),cloneOf:null};
  let confirmDel=false, q='';
  const draw=()=>{
    const pool=ents(S.uid).filter(e=>!L.items.some(x=>tkey(x)===tkey(e)) && (!q || e.title.toLowerCase().includes(q.toLowerCase()))).slice(0,6);
    sheet(`<div class="sh"><h2>${L.id?'Edit list':'New list'}</h2><button class="linkbtn" data-close style="color:var(--muted)">Cancel</button></div>
    <div class="field"><label for="l-name">Name</label><input id="l-name" maxlength="100" value="${esc(L.name)}" placeholder="Best of ${thisYear()}"></div>
    <div class="field"><label for="l-desc">Description</label><textarea id="l-desc" maxlength="1000" style="min-height:60px">${esc(L.desc)}</textarea></div>
    <label class="chk"><input type="checkbox" id="l-rank" ${L.ranked?'checked':''}> Ranked list (numbered)</label>
    <div class="sec"><h2>Items</h2><span class="ghost">${L.items.length}</span></div>
    ${L.items.map((x,n)=>`<div class="litem" data-type="${esc(x.type)}"><span class="mono ghost">${n+1}</span>${cover(x)}<span style="min-width:0;overflow-wrap:anywhere">${esc(x.title)} <span class="ghost">${esc(x.year||'')}</span></span><span style="display:flex"><button class="iconbtn" data-mv="${n}:-1" aria-label="Up">${I.up}</button><button class="iconbtn" data-mv="${n}:1" aria-label="Down">${I.down}</button><button class="iconbtn" data-rm="${n}" aria-label="Remove">${I.x}</button></span></div>`).join('')||'<p class="ghost">No items yet.</p>'}
    <div class="field"><label for="l-q">Add from your diary</label><input id="l-q" value="${esc(q)}" placeholder="Search your logs" autocomplete="off"></div>
    ${pool.map(e=>`<button class="optrow" data-add="${esc(e.id)}"><span>${esc(e.title)} <span class="ghost">${esc(e.year||'')} · ${TYPES[e.type].label}</span></span>${I.plus.replace('<svg','<svg width="18" height="18"')}</button>`).join('')}
    ${q.trim()?`<button class="optrow" data-addfree="1"><span>Add “${esc(q)}” as a new item</span>${I.plus.replace('<svg','<svg width="18" height="18"')}</button>`:''}
    <div class="actions">${L.id?`<button class="del" id="l-del">${confirmDel?'Tap again to delete list':'Delete list'}</button>`:'<span></span>'}<button class="btn pri" id="l-save">Save list</button></div>`, ()=>{
      const pull=()=>{ L.name=$('#l-name').value; L.desc=$('#l-desc').value; L.ranked=$('#l-rank').checked; };
      $('#l-q').oninput=e=>{ pull(); q=e.target.value; const p=e.target.selectionStart; draw(); const n=$('#l-q'); n.focus(); try{n.setSelectionRange(p,p);}catch(_){} };
      document.querySelectorAll('[data-mv]').forEach(b=>b.onclick=()=>{ pull(); const [n,dd]=b.dataset.mv.split(':').map(Number); const j=n+dd; if(j<0||j>=L.items.length) return; [L.items[n],L.items[j]]=[L.items[j],L.items[n]]; draw(); });
      document.querySelectorAll('[data-rm]').forEach(b=>b.onclick=()=>{ pull(); L.items.splice(+b.dataset.rm,1); draw(); });
      document.querySelectorAll('[data-add]').forEach(b=>b.onclick=()=>{ pull(); const e=find(S.uid,b.dataset.add); L.items.push({type:e.type,title:e.title,year:e.year||'',creator:e.creator||'',cover:e.cover||null}); q=''; draw(); });
      const af=$('[data-addfree]'); if(af) af.onclick=()=>{ pull(); L.items.push({type:'film',title:q.trim(),year:'',creator:'',cover:null}); q=''; draw(); };
      const dl=$('#l-del'); if(dl) dl.onclick=async()=>{ if(!confirmDel){ pull(); confirmDel=true; dl.textContent='Tap again to delete list'; return; } try{ await del(L.id); const pins=(prof(S.uid).pins||[]).filter(x=>x.id!==L.id); await saveProf({pins}); closeSheet(); toast('List deleted'); if(cur().v==='list') back(); }catch(e){ handleErr(e);} };
      $('#l-save').onclick=async()=>{ pull(); if(!L.name.trim()){ $('#l-name').focus(); return; }
        try{ const {id,...rest}=L; const nid=await put(id,{kind:'list',...rest,name:L.name.trim(),desc:L.desc.trim(),updatedAt:Date.now()}); closeSheet(); toast('List saved'); if(!id) go({v:'list',u:S.uid,i:nid}); }catch(e){ handleErr(e);} };
      if(!L.id && !L.name) $('#l-name').focus();
    });
  };
  draw();
}
function openToList(k){
  const t=titleMap().get(k)||exploreFind(k); if(!t) return;
  const ls=lists(S.uid);
  sheet(`<div class="sh"><h2>Add to list</h2><button class="linkbtn" data-close style="color:var(--muted)">Cancel</button></div><p class="hint">${esc(t.title)}</p>
    ${ls.map(l=>{ const has=(l.items||[]).some(x=>tkey(x)===k); return `<button class="optrow" data-tl="${esc(l.id)}" ${has?'disabled':''}><span><b>${esc(l.name)}</b><br><span class="hint">${(l.items||[]).length} items</span></span><span class="ghost">${has?'Added':'Add'}</span></button>`; }).join('')}
    <button class="optrow" id="tl-new"><span><b>New list…</b></span>${I.plus.replace('<svg','<svg width="18" height="18"')}</button>`, ()=>{
      document.querySelectorAll('[data-tl]').forEach(b=>b.onclick=async()=>{ const l=find(S.uid,b.dataset.tl); const {id,uid,...rest}=l;
        try{ await put(id,{...rest,items:[...(l.items||[]),{type:t.type,title:t.title,year:t.year||'',creator:t.creator||'',cover:t.cover||null}],updatedAt:Date.now()}); closeSheet(); toast('Added to '+l.name); }catch(e){ handleErr(e);} });
      $('#tl-new').onclick=()=>openList(null,{type:t.type,title:t.title,year:t.year||'',creator:t.creator||'',cover:t.cover||null});
    });
}

/* --- Import from Letterboxd --- */
function parseCSV(text){
  const rows=[]; let row=[],f='',q=false;
  for(let i=0;i<text.length;i++){ const c=text[i];
    if(q){ if(c==='"'){ if(text[i+1]==='"'){ f+='"'; i++; } else q=false; } else f+=c; }
    else if(c==='"') q=true; else if(c===','){ row.push(f); f=''; }
    else if(c==='\n'||c==='\r'){ if(c==='\r'&&text[i+1]==='\n') i++; row.push(f); rows.push(row); row=[]; f=''; }
    else f+=c; }
  if(f||row.length){ row.push(f); rows.push(row); }
  const head=(rows.shift()||[]).map(h=>h.replace(/^﻿/,'').trim());
  return rows.filter(r=>r.length>1).map(r=>Object.fromEntries(head.map((k,i)=>[k,(r[i]??'').trim()])));
}
function loadJSZip(){ return new Promise((res,rej)=>{ if(window.JSZip) return res(window.JSZip); const s=document.createElement('script'); s.src='https://cdnjs.cloudflare.com/ajax/libs/jszip/3.10.1/jszip.min.js'; s.onload=()=>res(window.JSZip); s.onerror=()=>rej(new Error('Could not load the unzip tool. Unzip the export and pick the CSV files instead.')); document.head.appendChild(s); }); }
function openImport(){
  sheet(`<div class="sh"><h2>Import from Letterboxd</h2><button class="linkbtn" data-close style="color:var(--muted)">Close</button></div>
    <p class="hint" style="font-size:13px">On letterboxd.com go to Settings → Data → Export your data. Then pick the .zip here (or the CSV files inside it). After the import, Artistry can look up posters for everything.</p>
    <label class="chk"><input type="checkbox" id="im-watched"> <span>Also add films marked watched that have no diary date<br><span class="hint">Uses the date you marked them.</span></span></label>
    <label class="chk"><input type="checkbox" id="im-watch" checked> Import watchlist</label>
    <div class="field"><label class="btn pri" for="im-file" style="align-self:flex-start">${I.upload} Choose export</label><input id="im-file" type="file" accept=".zip,.csv,application/zip,text/csv" multiple hidden></div>
    <div id="im-status" class="hint" style="margin-top:10px"></div><div class="progress" id="im-prog" hidden><i></i></div>`, ()=>{
      $('#im-file').onchange=async e=>{ const files=[...e.target.files]; if(!files.length) return; try{ await runImport(files); }catch(err){ $('#im-status').textContent=err.message||'Import failed.'; } };
    });
}
async function runImport(files){
  const st=$('#im-status'), pg=$('#im-prog'); st.textContent='Reading…';
  const csv={};
  for(const f of files){
    if(/\.zip$/i.test(f.name)||f.type.includes('zip')){ const JSZip=await loadJSZip(); const z=await JSZip.loadAsync(f);
      for(const [path,zf] of Object.entries(z.files)){ if(zf.dir||!/\.csv$/i.test(path)) continue; if(/(^|\/)(deleted|orphaned)\//i.test(path)) continue; const lp=path.toLowerCase(); csv[lp.includes('likes/')?'likes/'+lp.split('/').pop():lp.split('/').pop()]=await zf.async('string'); } }
    else csv[f.name.toLowerCase()]=await f.text();
  }
  const get=n=>csv[n]?parseCSV(csv[n]):[];
  const diary=get('diary.csv'), reviews=get('reviews.csv'), ratings=get('ratings.csv'), watched=get('watched.csv'), wl=get('watchlist.csv');
  const likedF=get('likes/films.csv').length?get('likes/films.csv'):get('films.csv');
  if(!diary.length&&!reviews.length&&!ratings.length&&!wl.length) throw new Error('No Letterboxd CSV files found. Choose the export .zip, or diary.csv / ratings.csv / reviews.csv / watchlist.csv.');
  const strip=s=>s.replace(/<br\s*\/?>/gi,'\n').replace(/<\/p>\s*<p>/gi,'\n\n').replace(/<[^>]+>/g,'').replace(/&amp;/g,'&').replace(/&quot;/g,'"').replace(/&#39;|&#039;/g,"'").replace(/&lt;/g,'<').replace(/&gt;/g,'>').trim();
  const E=new Map(); const ny=r=>r.Name+'|'+r.Year;
  const mk=(r,date)=>({kind:'entry',type:'film',title:r.Name,creator:'',year:r.Year||'',date:(date||r.Date||today()).slice(0,10),rating:+r.Rating||0,liked:false,rewatch:r.Rewatch==='Yes',owned:false,spoilers:false,service:'',tags:(r.Tags||'').split(',').map(s=>s.trim()).filter(Boolean),review:'',cover:null});
  diary.forEach(r=>{ E.set(ny(r)+'|'+(r['Watched Date']||r.Date),mk(r,r['Watched Date'])); });
  reviews.forEach(r=>{ const k=ny(r)+'|'+(r['Watched Date']||r.Date); const e=E.get(k)||mk(r,r['Watched Date']); e.review=strip(r.Review||''); if(/spoiler/i.test(r.Review||'')) e.spoilers=true; E.set(k,e); });
  const seen=new Set([...E.values()].map(e=>e.title+'|'+e.year));
  ratings.forEach(r=>{ if(!seen.has(ny(r))){ E.set(ny(r)+'|r',mk(r,r.Date)); seen.add(ny(r)); } });
  if($('#im-watched').checked) watched.forEach(r=>{ if(!seen.has(ny(r))){ const e=mk(r,r.Date); e.rating=0; E.set(ny(r)+'|w',e); seen.add(ny(r)); } });
  const likedSet=new Set(likedF.map(ny)); E.forEach(e=>{ if(likedSet.has(e.title+'|'+e.year)) e.liked=true; });
  const existing=new Set(ents(S.uid).map(e=>tkey(e)+'|'+e.date));
  const out=[...E.values()].filter(e=>!existing.has(tkey(e)+'|'+e.date));
  const wOut = $('#im-watch').checked ? wl.filter(r=>!watch(S.uid).some(w=>tkey(w)===tkey({type:'film',title:r.Name,year:r.Year}))) : [];
  let rank=Math.max(0,...watch(S.uid).map(x=>x.rank||0));
  const jobs=[...out.map((e,i)=>({...e,createdAt:Date.now()-out.length+i,updatedAt:Date.now()})), ...wOut.map(r=>({kind:'watch',type:'film',title:r.Name,year:r.Year||'',creator:'',service:'',cover:null,rank:++rank,createdAt:Date.parse(r.Date)||Date.now()}))];
  if(!jobs.length){ st.textContent='Everything in this export is already here.'; return; }
  pg.hidden=false; let done=0; const total=jobs.length;
  const tick=()=>{ st.textContent=`Importing ${done} of ${total}…`; pg.firstElementChild.style.width=(done/total*100)+'%'; }; tick();
  const entryRows=jobs.filter(j=>j.kind==='entry').map(j=>({...toRow('entry',j),user_id:S.uid,created_at:iso(j.createdAt)}));
  const watchRows=jobs.filter(j=>j.kind==='watch').map(j=>({...toRow('watch',j),user_id:S.uid,created_at:iso(j.createdAt)}));
  for(const [table,rows] of [['entries',entryRows],['watchlist',watchRows]]){
    for(let n=0;n<rows.length;n+=200){ const chunk=rows.slice(n,n+200); const {error}=await sb.from(table).insert(chunk);
      if(error){ st.textContent='Import stopped: '+friendly(error); await loadAll(); return; } done+=chunk.length; tick(); } }
  await loadAll();
  st.innerHTML=`Imported ${done} items. <button class="linkbtn" id="im-fill">Find covers for them</button>`; toast('Import finished');
  $('#im-fill').onclick=()=>fillPosters(st);
}

/* ---------- Rendering helpers ---------- */
let rT; function scheduleRender(){ clearTimeout(rT); rT=setTimeout(()=>{ const a=document.activeElement; if(a && ['cnew','cedit','q'].includes(a.id) && a.value){ softRender(); return; } render(); },60); }
function softRender(){ const a=document.activeElement; const id=a&&a.id, v=a&&a.value, p=a&&a.selectionStart; render(); const n=id&&document.getElementById(id); if(n){ if(typeof v==='string') n.value=v; n.focus(); try{ n.setSelectionRange(p,p);}catch(e){} } }

/* ---------- Sign in ---------- */
function signInV(){
  if(S.fatal) return `<div class="signin"><div class="biglogo">${LOGO}</div><h2>Almost there</h2><p class="ghost">${esc(S.fatal)}</p></div>`;
  if(!S.booted) return `<div class="signin"><div class="biglogo">${LOGO}</div><p class="ghost">Loading…</p></div>`;
  if(S.siSent) return `<div class="signin"><div class="biglogo">${LOGO}</div><h2>Check your email</h2><p class="ghost">We sent a sign-in link to <b>${esc(S.siSent)}</b>. Open it on this device to finish signing in.</p><button class="linkbtn" data-a="si-reset">Use a different email</button></div>`;
  return `<div class="signin"><div class="biglogo">${LOGO}</div><h2>Artistry</h2><p class="ghost">A diary for the films, shows, books and albums you finish, shared with friends.</p>
    <form id="si-form" class="siform"><label for="si-email" class="flabel">Email</label><input id="si-email" type="email" autocomplete="email" required placeholder="you@example.com"><button class="btn pri" type="submit">Email me a sign-in link</button></form>
    <p class="hint">No password needed. New here? The same link creates your account.</p></div>`;
}
document.addEventListener('submit', async ev=>{
  if(ev.target.id!=='si-form') return; ev.preventDefault();
  const email=$('#si-email').value.trim(); const b=ev.target.querySelector('button'); b.disabled=true; b.textContent='Sending…';
  const {error}=await sb.auth.signInWithOtp({email,options:{emailRedirectTo:location.origin+location.pathname}});
  if(error){ b.disabled=false; b.textContent='Email me a sign-in link'; toast(error.message); return; }
  S.siSent=email; render();
});

/* ---------- Onboarding ---------- */
function openOnboard(){
  const p=prof(S.uid);
  sheet(`<div class="sh"><h2>Welcome to Artistry</h2></div><p class="hint">Pick a username. It’s how friends find you and appears in your profile link.</p>
    <div class="field"><label for="ob-handle">Username</label><input id="ob-handle" maxlength="30" autocomplete="username" autocapitalize="none" value="${esc((p.display||'').toLowerCase().replace(/[^a-z0-9_.]/g,'').slice(0,30))}"></div>
    <div class="field"><label for="ob-name">Name</label><input id="ob-name" maxlength="50" value="${esc(p.display||'')}"></div>
    <div id="ob-err" class="err" hidden></div>
    <div class="actions"><span></span><button class="btn pri" id="ob-save">Start my diary</button></div>`, ()=>{
    $('#ob-handle').oninput=e=>{ e.target.value=e.target.value.toLowerCase().replace(/[^a-z0-9_.]/g,''); };
    $('#ob-save').onclick=async()=>{ const h=$('#ob-handle').value.trim(); const er=$('#ob-err');
      if(!/^[a-z0-9_.]{2,30}$/.test(h)){ er.hidden=false; er.textContent='Use 2–30 lowercase letters, numbers, dots or underscores.'; return; }
      try{ await saveProf({handle:h,display:$('#ob-name').value.trim()}); closeSheet(); toast('Welcome, @'+h); render(); }catch(e){ er.hidden=false; er.textContent=friendly(e); } };
  }, {locked:true});
}

/* ---------- Deep links (#@username) ---------- */
function openHash(){
  const m=location.hash.match(/^#@([a-z0-9_.]{2,30})$/); if(!m) return;
  const uid=Object.keys(S.prof).find(u=>S.prof[u].handle===m[1]); if(uid) tab({v:'profile',u:uid,t:'profile'});
}
window.addEventListener('hashchange',openHash);

/* ---------- Fill in missing posters ---------- */
async function fillPosters(statusEl){
  const todo=ents(S.uid).filter(e=>!e.cover);
  if(!todo.length){ statusEl.textContent='Every entry already has a cover.'; return; }
  let done=0, found=0;
  const norm=s=>String(s||'').toLowerCase().replace(/[^a-z0-9]/g,'');
  for(const e of todo){
    statusEl.textContent=`Looking up ${done+1} of ${todo.length}…`;
    try{
      const r=await api(`/api/search?type=${e.type}&q=${encodeURIComponent(e.title)}${e.type==='film'&&e.year?'&year='+e.year:''}`);
      const res=r.results||[];
      const hit=res.find(x=>norm(x.title)===norm(e.title)&&(!e.year||!x.year||x.year===e.year)) || res.find(x=>norm(x.title)===norm(e.title));
      if(hit && hit.poster){
        const row={poster_url:hit.poster,ext_id:hit.ext||null};
        if(!e.backdrop && hit.backdrop) row.backdrop_url=hit.backdrop;
        if(!e.creator && hit.creator) row.creator=hit.creator;
        if(!e.year && hit.year) row.year=hit.year;
        const {error}=await sb.from('entries').update(row).eq('id',e.id); if(!error) found++;
      }
    }catch(err){ /* skip this one */ }
    done++; await new Promise(r=>setTimeout(r,120));
  }
  statusEl.textContent=`Found covers for ${found} of ${todo.length}.`; await loadAll();
}

/* ---------- Boot ---------- */
async function boot(){
  render();
  let cfg;
  try{ cfg=await api('/api/config'); }catch(e){ S.fatal='Setup isn’t finished: '+e.message; S.booted=true; render(); return; }
  S.tmdb=cfg.tmdb;
  sb=createClient(cfg.supabaseUrl,cfg.supabaseAnonKey,{auth:{persistSession:true,autoRefreshToken:true,detectSessionInUrl:true}});
  const {data:{session}}=await sb.auth.getSession();
  S.uid=session?.user?.id||null; S.canWrite=!!S.uid; S.booted=true;
  sb.auth.onAuthStateChange((_ev,sess)=>{
    const id=sess?.user?.id||null; if(id===S.uid) return;
    S.uid=id; S.canWrite=!!id; S.items={}; S.prof={}; S.follows=[]; S.ready=false; S.stack=[{v:'home'}];
    if(id) startData(); else render();
  });
  loadExplore();
  if(S.uid) startData(); else render();
}
async function startData(){
  render();
  await loadAll();
  if(!S.rt) S.rt=sb.channel('artistry-db').on('postgres_changes',{event:'*',schema:'public'},()=>scheduleLoad()).subscribe();
  if(!prof(S.uid).handle) openOnboard(); else openHash();
}
boot();
