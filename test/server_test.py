import json
from http.server import BaseHTTPRequestHandler, HTTPServer

class SimpleHTTPRequestHandler(BaseHTTPRequestHandler):
    def do_GET(self):
        if self.path == "/data":
            # Load the JSON data from the file
            with open("data.json") as f:
                data = json.load(f)
            
            # Set the response code to 200 (OK)
            self.send_response(200)
            # Specify that the content type is JSON
            self.send_header("Content-type", "application/json")
            self.end_headers()
            
            # Write the JSON data as the response
            self.wfile.write(json.dumps(data).encode("utf-8"))
        else:
            # If any other path is requested, send a 404 Not Found response
            self.send_response(404)
            self.end_headers()
            self.wfile.write(b"404 Not Found")

# Set the server address and port
server_address = ("", 8000)  # "" means localhost, port 8000
httpd = HTTPServer(server_address, SimpleHTTPRequestHandler)

print("Server running on http://127.0.0.1:8000")
httpd.serve_forever()

