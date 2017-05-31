from sklearn.datasets.samples_generator import make_blobs

X, y = make_blobs(n_samples=5000, centers=7, n_features=2, random_state=3)

#print(X)
#print(y)
colors = [
	(222/255, 93/255, 90/255),
	(110/255, 166/255, 52/255),
	(60/255, 83/255, 111/255),
	(249/255, 225/255, 126/255),
	(143/255, 221/255, 234/255),
	(225/255, 236/255, 113/255),
	(106/255, 90/255, 127/255),
	(219/255, 201/255, 234/255),
	(128/255, 84/255, 98/255)
	]

f = open('test.csv', 'w')

maxx = 0
for x in X:
	if x[0] > maxx:
		maxx = x[0]
	if x[1] > maxx:
		maxx = x[1]
minn = 10000
for x in X[:]:
	if x[0] < minn:
		minn = x[0]
	if x[1] < minn:
		minn = x[1]

print(minn)
print(maxx)
srange = (maxx - minn) / 2

for i in range(0, len(X)):
	x = X[i]
	c = y[i]
	f.write('%f, %f, %f, %f, %f\n' % (x[0]/srange, x[1]/srange, colors[c][0], colors[c][1], colors[c][2]))

f.close()


