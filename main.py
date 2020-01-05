import camera
import ubinascii
import usocket as socket

import ulogging as logging
logging.basicConfig(level=logging.INFO)
log = logging.getLogger(__name__)


# Initialize socket connection
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.bind(('', 80))
s.listen(1)

# Check for incoming request
while True:
  conn, addr = s.accept()

  # Initialize camera
  camera.init()

  # capture Image
  data = camera.capture()

  conn.send('HTTP/1.1 200 OK\n')
  conn.send('Content-Type: application/json\n\n')

  # Convert to base64
  img = ubinascii.b2a_base64(data)

  # Send Image back
  conn.send(img)

  # Deinitialize camera
  camera.deinit()

  conn.close()