from urllib.request import urlopen
import jieba
#Retrieve HTML string from the URL
html = urlopen("http://www.pythonscraping.com/exercises/exercise1.html")
print(list(jieba.cut(html.read())))
