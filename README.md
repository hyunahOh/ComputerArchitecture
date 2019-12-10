# ComputerArchitecture

• 1) Invoke init_memory_content();
• 2) Invoke init_cache_content();
• 3) open “access_input.txt” file
• 4) Read each line
• 5) Try to access the cache to check whether the data
is available in the cache;
If so, access it directly from the cache
• 6) Otherwise, attempt to access the main memory and
update the cache
• 7) Print the accessed data into “access_output.txt”
• 8) Increment global_timestamp by 1
• 9) Go to 4)
