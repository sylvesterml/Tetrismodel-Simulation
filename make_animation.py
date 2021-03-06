import matplotlib.pyplot as plt
import matplotlib.animation as animation
import pandas as pd

model_data = pd.read_csv("./tetrismodel.csv")

count = 83 * 83

fig = plt.figure(figsize=(8, 8))
ims = []
for i in range(1000):
    data = model_data[:][i*count:(i+1)*count-1]
    im = plt.scatter(data['x'], data['y'], s=10, c=data['color'], marker='o')
    ims.append([im])

ani = animation.ArtistAnimation(fig, ims, interval=50, repeat=1)
ani.save('tetrismodel.mp4')

plt.show()

