import random
import csv 

names = []

fatentry = "A" * 30
rows = 500
cols = 15
name_idx = 9

for n in range(rows):
  rand = random.randint(0,100)
  if rand < 75:
    names.append(random.randint(1,4))
  else:
    names.append(random.randint(5, 1000000)) 

output = ",tweet_id,airline_sentiment,airline_sentiment_confidence,negativereason,negativereason_confidence,airline,airline_sentiment_gold,negativereason_gold,name,retweet_count,text,tweet_coord,tweet_created,tweet_location,user_timezone"
for n in range(rows):
  row = ""
  for c in range(cols+1):
    if c == name_idx:
      rand = random.randint(0,100)
      name = names[rand % len(names)]
      row += str(name)
    else:
      row += fatentry
    
    if c != cols:
      row += ","

  output += "\n" + row

print output


