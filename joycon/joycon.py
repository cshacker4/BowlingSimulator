from pyjoycon import JoyCon, get_R_id
import mmap
import os
import time
import json

def is_valid_dictionary(data):
    if not isinstance(data, dict):
        return False  # Not a dictionary
    
    # Check for valid key-value pairs
    for key, value in data.items():
        if not isinstance(key, (str, int, float)):
            return False  # Key is not a string or number
        if not isinstance(value, (str, int, float, dict, list)):
            return False  # Value is not a string, number, dictionary, or list
        
        # Recursively validate nested dictionaries
        if isinstance(value, dict):
            if not is_valid_dictionary(value):
                return False  # Nested dictionary is not valid
            
        # Optionally, validate other types of values
        
    return True  # Dictionary is valid

# Create joycon object
joycon_id = get_R_id()
joycon = JoyCon(*joycon_id)
#example var: joycon_stats = joycon.get_status()

shared_memory_path = "./tmp/joycon_status.json"

# Create the shared memory file if it doesn't exist
if not os.path.exists(shared_memory_path):
    with open(shared_memory_path, 'wb') as f:
        f.write(b'\0' * mmap.PAGESIZE)  # Initialize the file with null bytes to ensure correct size

# Open the shared memory file
with open(shared_memory_path, 'r+b') as f:
    # Memory-map the file
    memory_map = mmap.mmap(f.fileno(), 0)
    try:
        while True:
            joycon_status = joycon.get_status()
            json_str = json.dumps(joycon_status)
            json_str += '\0'  # Null-terminate the string
            json_bytes = json_str.encode('utf-8')

            # Write data to shared memory
            memory_map.seek(0)
            memory_map.write(json_bytes)

            #Wait for 15ms
            time.sleep(0.015)

        #time.sleep(0.015)
    except KeyboardInterrupt:
            print("KeyboardInterrupt received. Cleaning up resources...")

#close the file
memory_map.close()
f.close()
