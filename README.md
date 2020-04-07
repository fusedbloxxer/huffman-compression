# huffman-compression
During a faculty course called digital computers I have developed a project that calculates the Shannon Information/Entropy for text sequences/words and which compresses the information using the Huffman-Tree algorithm and it's also able to decode it. The compression uses characters' frequency in a source text in order to create a strict binary tree which holds the the encryption data. To optimize compression I used a hash-map to store the related character-code pairs which gives an O(1) access or O(n * log2(n)) as I am using balanced trees to deal with collisions. The decoding has an O(log2(n)) complexity because the encryted binary digits(0, 1) have to traverse the the tree to find the leaf node and get the answer. 

The project can be extended to support word-based compression or longer sequences, however one would have to implement the corresponding functions in order to pass them as arguments in my generic API.

Techniques that were used to develop this project include: bit arrays, binary files, generic hash-tables with balanced generic binary, generic heaps and much more. 

To test the encryption place the text inside the plain_text.txt file. The encrypted/compressed information will be available in encrypted_text.txt and the decrypted text will be generated in decrypted_text.txt after executing the program.

The Huffman Tree will be available in the console output as well as the hash-table containing the key-code pairs.
