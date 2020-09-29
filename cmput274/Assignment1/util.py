"""
Name: LOGAN VAUGHAN
ID #:  1577489
CMPUT  274  EA1

Assignment 1: Huffman Coding
"""



import bitio
import huffman
import pickle



def read_tree(tree_stream):
    """
    Using the pickle module, reads a description of a Huffman tree from
    'tree_stream'. Then constructs the Huffman tree object and returns its 
    root node.

    Args:
      tree_stream: The compressed stream the Huffman tree is read from.

    Returns:
      A Huffman tree root constructed according to the description in tree_stream.
    """
    
    uncompressed_tree = pickle.load(tree_stream)

    return uncompressed_tree



def decode_byte(tree, bitreader):
    """
    Reads bits from the bitreader and uses them to traverse the given Huffman
    tree 'tree' from root to leaf. When a leaf is found, the value of that leaf 
    is returned.

    Args:
      bitreader: An instance of bitio.BitReader that reads encoded bits into the
      	program so that they can be decoded by 'tree'.
      tree: A Huffman tree.

    Returns:
      A decoded byte (the next byte in the compressed bit stream of 'bitreader').
    """

    x = 0
    while x == 0:

      if isinstance(tree, huffman.TreeBranch):
        if bitreader.readbit() == 0:
          tree = tree.getLeft()
        else:
          tree = tree.getRight()

      if isinstance(tree, huffman.TreeLeaf):
        decoded_byte = tree.getValue()
        x = 1

    return decoded_byte



def decompress(compressed, uncompressed):
    """
    Uses the function 'read_tree' to get a Huffman tree from the encoded
    stream 'compressed'. Then uses that Huffman tree to decode the rest to the
    stream 'compressed' with the help of the function 'decode_byte'. Nothing
    is returned, but the decoded bytes from 'decode_byte' are writted to the 
    stream 'uncompressed'.

    Args:
      compressed: A file stream from which a compressed input is read.
      uncompressed: A writable file stream to which the uncompressed output is 
      	written.
    """

    huffman_tree = read_tree(compressed)

    decomp_reader = bitio.BitReader(compressed)
    decomp_writer = bitio.BitWriter(uncompressed)

    stop_reading = False
    while not(stop_reading):
      byte = decode_byte(huffman_tree, decomp_reader)
      if byte is None:
        stop_reading = True
      else:
        decomp_writer.writebits(byte, 8)

    decomp_writer.flush()



def write_tree(tree, tree_stream):
    """
    Writes a Huffman tree 'tree' to the stream 'tree_stream'
    using pickle.

    Args:
      tree: A Huffman tree.
      tree_stream: The binary file that 'tree' is written to.
    """

    pickle.dump(tree, tree_stream)



def compress(tree, uncompressed, compressed):
    """
	Writes a Huffman tree 'tree' to the stream 'compressed' using the
	function 'write_tree'. Then uses the same Huffman tree 'tree' to
	encode the bits from the input stream 'uncompressed' and write the
	new encoded data to 'compressed'.

    Args:
      tree: A Huffman tree.
      uncompressed: A file stream from which a uncompressed input is read.
      compressed: A writable file stream to which the compressed output is 
      	written.
    """
    
    write_tree(tree, compressed)

    comp_reader = bitio.BitReader(uncompressed)
    comp_writer = bitio.BitWriter(compressed)
    table = huffman.make_encoding_table(tree)

    eof = False
    while not(eof):
      try:
        for bit in table[comp_reader.readbits(8)]:
          comp_writer.writebit(bit)

      except EOFError:
        eof = True

    comp_writer.flush()