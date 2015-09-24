# CS410-HW2

This program copies files, as specified in the protocol. 
It takes three command-line arguments (buffer size, file to copy from, and file to copy into).

It also measures the performance as buffer-size is varied. The data shows that in general, the I/O was
more efficient with large buffer sizes up to a point, but with larger buffer sizes, the time increased.
Perhaps this resulted in fewer disk operations.
No clear patterm emerged about the advantages of powers of two or multiples of large powers of two.
