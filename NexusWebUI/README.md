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
5. Configure the Application via the .env file
   1. To make it easier, a .env_example file was added to see the general form of the file
   2. Generate the Secret Key (This should stay secret in Prod as the name suggests)
      1. Create a File called .env in the same folder as manage.py or rename the.env_example file to .env
      2. Open it in a Text Editor
      3. Run the following command in the shell:
         1. python -c "import secrets; print(secrets.token_urlsafe())"
      4. Create a line with the secret key you got from above command in the .env file
         1. Example: SECRET_KEY=123123$%/$%ghjhghjk
   3. Add your Server IP to the "Allowed Hosts" - Settings in .env
      1. Example: ALLOWED_HOSTS=192.168.1.1, 
         1. (This is optional, but highly recommended, you can add more than one IP splitting the entries with a comma)
   4. Add the IP of the Server running the Pool 
      1. Example: POOL_IP=10.1.1.55
   5. Add the Port of the Server running the Pool
      1. Example: POOL_PORT=5000
6. Run the necessary Migrations (from NexusWebUI Folder)
python manage.py migrate
7. In settings.py configure the IP of your server
8. Run Testserver (for example python ./manage.py runserver 0.0.0.0:8000 to run it on port (8000))
9. python manage.py runserver


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


## Settings
All local Settings are to be done in the .env file

