# OTP-Cryptography

Final Project for Operating Stystems 1 (CS_344_400_F2017)

Five programs that encrypt and decrypt information using a one-time pad-like system

keygen.c: 

 	This program creates a key of specified length. The characters generated will be any of the 27 allowed characters, generated using the standard UNIX randomization methods.

otp_enc.c:

	Client application that connects to otp_enc_d, and asks it to perform a one-time pad style encryption. 

otp_enc_d.c: 

	This program is to be ran in the background as a daemon. Its function is to perform actual encoding. This program will listen on a particular socket/port. When a connection is made, it will spawn a child process to handle the client requet. The child receives a plaintext and key via communication socket, and will write back the ciphertext.

otp_dec.c:

	Client application that connects to otp_dec_d, and asks it to perform a one-time pad style decryption. 

otp_dec_d.c:

	This program is to be ran in the background as a daemon. Its function is to perform actual decoding. This program will listen on a particular socket/port. When a connection is made, it will spawn a child process to handle the client requet. The child receives an encrypted text and key via communication socket, and will write back the decyphered text.

----------------------------------------------------------------------------------------------------------------------------------------

Example usage with bash:

	$ otp_enc_d 50011 &
	
	$ otp_dec_d 50012 &
	
	$ keygen 50 > mykey
	
	$ cat plaintext1
	
	$ THE RED GOOSE FLIES AT MIDNIGHT STOP
	
	$ otp_enc plaintext1 mykey 50011 > ciphertext1
	
	$ cat ciphertext1
	
	$ FJLKAESJOL IOVJSANFL PAOIEFJA DYGFYB
	
	$ otp_dec ciphertext1 mykey 50012 > plaintext1_a
	
	$ cat plaintext1_a
	
	$ THE RED GOOSE FLIES AT MIDNIGHT STOP
