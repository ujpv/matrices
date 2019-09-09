def writeData(a, file_name):
    with open(file_name, 'w') as f:
        f.write(' '.join(map(str, a)))
        
def makeData(size): 
    a = [] 
    for i in range(size * size * 2): 
        a.append(random.random()) 
    return a 
