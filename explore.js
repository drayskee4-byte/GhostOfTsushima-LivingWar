import { send, getJSON, tmdb, hasTMDB, movieItem, tvItem, bookItem, albumItem, bigArt } from './_lib.js';

// GET /api/explore — live charts for the Explore tab. Each section is optional: if a source fails, it is skipped.
async function books() {
  try {
    const r = await getJSON('https://openlibrary.org/trending/weekly.json?limit=20');
    const w = (r.works || []).filter(x => x.cover_i).map(bookItem);
    if (w.length) return w;
  } catch (e) { /* fall through */ }
  const r = await getJSON('https://openlibrary.org/search.json?' + new URLSearchParams({ q: 'trending_score_hourly_sum:[1 TO *]', sort: 'trending', limit: '20', fields: 'key,title,author_name,first_publish_year,cover_i' }));
  return (r.docs || []).filter(x => x.cover_i).map(bookItem);
}
async function albums() {
  try {
    const r = await getJSON('https://rss.applemarketingtools.com/api/v2/us/music/most-played/25/albums.json');
    const a = (r.feed?.results || []).map(x => ({ type: 'music', ext: 'itunes:' + x.id, title: x.name, creator: x.artistName, year: (x.releaseDate || '').slice(0, 4), poster: bigArt(x.artworkUrl100), backdrop: null, note: '' }));
    if (a.length) return a;
  } catch (e) { /* fall through */ }
  const r = await getJSON('https://api.deezer.com/chart/0/albums?limit=25');
  return (r.data || []).map(x => ({ type: 'music', ext: 'deezer:' + x.id, title: x.title, creator: x.artist?.name || '', year: '', poster: x.cover_xl || x.cover_big || null, backdrop: null, note: '' }));
}

export default async function handler(req, res) {
  const tasks = {
    films: hasTMDB() ? tmdb('/trending/movie/week').then(r => r.results.map(movieItem)) : Promise.reject(new Error('TMDB not configured')),
    shows: hasTMDB() ? tmdb('/trending/tv/week').then(r => r.results.map(tvItem)) : Promise.reject(new Error('TMDB not configured')),
    books: books(),
    albums: albums(),
  };
  const keys = Object.keys(tasks);
  const out = await Promise.allSettled(Object.values(tasks));
  const got = {}; const errors = {};
  out.forEach((s, i) => { if (s.status === 'fulfilled') got[keys[i]] = s.value.filter(x => x.title).slice(0, 16); else errors[keys[i]] = String(s.reason?.message || s.reason); });
  const sections = [];
  if (got.films?.length) sections.push({ type: 'film', title: 'Popular movies this week', source: 'Trending on TMDB this week', items: got.films });
  if (got.shows?.length) sections.push({ type: 'tv', title: 'Popular shows this week', source: 'Trending on TMDB this week', items: got.shows });
  if (got.books?.length) sections.push({ type: 'book', title: 'Popular books this week', source: 'Trending on Open Library this week', items: got.books });
  if (got.albums?.length) sections.push({ type: 'music', title: 'Popular music this week', source: 'Most played albums on Apple Music', items: got.albums });
  const spotlight = (got.films || []).find(f => f.backdrop) || null;
  send(res, 200, { updated: new Date().toISOString(), spotlight, sections, errors }, 3600);
}
