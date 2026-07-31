import http.server
import socketserver
import urllib.parse
import os

class FractalHandler(http.server.SimpleHTTPRequestHandler):
    def do_GET(self):
        if self.path.startswith('/api/render'):
            query = urllib.parse.urlparse(self.path).query
            params = urllib.parse.parse_qs(query)
            
            mode = params.get('mode', ['image'])[0]
            z    = params.get('zoom', ['1'])[0]
            x    = params.get('x', ['-0.75'])[0]
            y    = params.get('y', ['0.0'])[0]
            f    = params.get('formula', ['mandelbrot'])[0]
            p    = params.get('palette', ['dstep'])[0]
            it   = params.get('iter', ['150'])[0]
            sec  = params.get('sec', ['3'])[0]
            
            os.system(f'./render_engine {mode} {z} {x} {y} {f} {p} {it} {sec}')
            
            self.send_response(200)
            self.send_header('Content-type', 'text/plain')
            self.end_headers()
            self.wfile.write(b"OK")
        else:
            super().do_GET()

PORT = 8080
socketserver.TCPServer.allow_reuse_address = True
with socketserver.TCPServer(("127.0.0.1", PORT), FractalHandler) as httpd:
    print(f"🚀 Сервер запущен на http://127.0.0.1:{PORT}")
    httpd.serve_forever()
