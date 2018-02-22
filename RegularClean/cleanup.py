import schedule
import time
import os

def job():
    os.system("rm -rf /home/tmp/*")

schedule.every().day.do(job)

while True:
    schedule.run_pending()
    time.sleep(1)