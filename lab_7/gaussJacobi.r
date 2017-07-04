
A <- matrix(c(1,2,0,1,2,2,1,-1,-3,-2,1,1,7,3,0,-2),4,4)
b <- matrix(c(12,10,-1,-4),4,1)
x <- x_new <- matrix(0,4,1)

A <- matrix(c(5,-3,2,-2,9,-1,3,1,-7),3,3)
b <- matrix(c(-1,2,3),3,1)
x <- x_new <- matrix(0,3,1)

for (itr in 1:10) {

for (i in 1:nrow(A)) {
	x_new[i] <- b[i]
	for (j in 1:nrow(A))
		if (i!=j)
			x_new[i] = x_new[i] - A[i,j]*x[j]
	x_new[i] <- x_new[i] / A[i,i]
}
print(x_new)
x <- x_new

}
