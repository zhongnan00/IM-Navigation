import fs from 'fs';
import Koa from 'koa';
import http from 'http';
import serve from 'koa-static';
import mime from 'mime-types';
import path from 'path';
import Router from '@koa/router';
import { fileURLToPath } from 'url';
import cors from '@koa/cors';


const __filename = fileURLToPath(import.meta.url);
const __dirname = path.dirname(__filename);
const main = serve(path.join(__dirname, '..', '/build'), {
  maxage: 31557600000,
});

const app = new Koa();
const router = new Router();

app.use(main);
router.get('(.html)', (ctx) => {
  const _path = path.join(__dirname,'..'  ,'/build/index.html');
  const src = fs.createReadStream(_path);
  const mimeType = mime.lookup(_path);
  ctx.response.set('content-type', mimeType || 'text/html');
  ctx.body = src;
});
app.use(cors());

const httpserver = http.createServer(app.callback());

httpserver.listen(
  3000,
  () => {
    console.log('http server failed to start on port 3000');
  },
  () => {
    console.log('http server listening on port 3000');
  }
);


