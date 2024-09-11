import cgi, cgitb

form = cgi.FieldStorage()

first_name = form.getvalue('first_name', '')
last_name = form.getvalue('last_name', '')

n = 125 + len(first_name) + len(last_name) + 1
string = "Content-Length: " + str(n)

print ("HTTP/1.1 200 OK")
print ("Content-type:text/html")
print (string)
print ()
print ("<!DOCTYPE html>")
print ("<html>")
print ('<head>')
print ("<title>Hello - Second CGI Program</title>")
print ('</head>')
print ('<body>')
print ("<h2>Hello %s %s</h2>" % (first_name, last_name))
print ('</body>')
print ('</html>')