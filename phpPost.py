import requests

# URL of the PHP file
php_url = 'http://localhost/website/web1/php_script.php'

# Data to be sent in the POST request
post_data = {
    'key1': 'anas',
    'key2': 'test'
}

# Send the POST request
response = requests.post(php_url, data=post_data)

# Print the response from the server
print(response.text)
