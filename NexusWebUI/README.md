# NexusPool WEB-UI

## Basic Server Setup

The Server is designed to work on Windows as well as Unix/Linux \
The final Setup depends highly on the chosen approach regarding the final Environment\

1. Install Python >3.6 from https://www.python.org/
2. Recommended: Create a Virtual Environment in the NexusWebUI Folder 
   1. Example Usage:
   https://uoa-eresearch.github.io/eresearch-cookbook/recipe/2014/11/26/python-virtual-env/
3. Open a shell capable of running Python in the NexusWebUI Folder
4. Install required Python Libraries
pip install -r requirements.txt
(This downloads the necessary Dependencies)
5. Generate the Secret Key (This should stay secret in Prod as the name suggests)
   1. Create a File called .env in the same folder as manage.py
   2. Open it in the a Text Editor
   3. Run the following command in the shell:
      1. python -c "import secrets; print(secrets.token_urlsafe())"
   4. Create a line with the secret key you got from above command in the .env file
      1. Example: SECRET_KEY=123123$%/$%ghjhghjk
6. Run the necessary Migrations (from NexusWebUI Folder)
python manage.py migrate
7. Run Testserver
8. python manage.py runserver


## Recommended Setup
The above setup should only be used in a small scale Test Environment due its poor scaling and security
, instead it is highly  recommended to use the following or a similar setup
1. Install a WSGI Server and configure accordingly:
   1. For Windows https://docs.pylonsproject.org/projects/waitress/en/latest/
   2. For Linux/Unix: https://gunicorn.org/
2. Run your WSGI Server as a Service, examples 
   1. Windows: https://www.devdungeon.com/content/run-python-script-windows-service
   2. Linux/Unix: http://supervisord.org/
      1. There is an example config in the Gunicorn Documentation
3. Use a proper Webserver like NGINX or Tomcat to handle higher loads and heighten security:
   1. https://nginx.org/
   2. http://tomcat.apache.org/
4. Use SSL to provide secure user access

## Tutorials
1. https://medium.com/analytics-vidhya/dajngo-with-nginx-gunicorn-aaf8431dc9e0
2. https://www.youtube.com/watch?v=BBKq6H9Rm5g

