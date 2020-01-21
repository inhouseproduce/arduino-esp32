import camera
import machine
import ubinascii
import usocket as socket
import utime

# Initialize socket connection
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.bind(('', 80))
s.listen(1)

while True:
  conn, addr = s.accept()

  # Params
  params = conn.recv(1024).decode()

  # return object property by name (param is string)
  def obj_props(arg):
    data = params.split(arg)
    if len(data) >= 2:
      return (data[1].split(':'))[1].split(',')[0].split('}')[0]
    else: 
      return 'none'

  conn.send('HTTP/1.1 200 OK\n')
  conn.send('Content-Type: application/json\n\n')

  if obj_props('scan') != 'none':
    conn.send('true')

  elif obj_props('capture') != 'none':
    # Initialize camera
    camera.init()

    # delay 1 sec. lense to be ready
    utime.sleep_us(1000)

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


  # Close connection
  conn.close()


  # Put esp to sleep (miliseconds)
  # if sleep_time:
  #   machine.deepsleep(60000 * int(sleep_time))