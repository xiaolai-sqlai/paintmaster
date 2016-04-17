#coding=utf-8
from celery import Celery
from sqlalchemy import create_engine, Column
from sqlalchemy.types import String, Integer, Date
from sqlalchemy.orm import sessionmaker
from sqlalchemy.ext.declarative import declarative_base
import os, datetime, sys, subprocess

reload(sys)
sys.setdefaultencoding("utf-8")

Base = declarative_base()

celery = Celery('tasks',broker="redis://127.0.0.1:6379/0")

class Images(Base):
    __tablename__ = 'images'
    id = Column(Integer, primary_key=True)
    src_img = Column(String(255))
    finish_img = Column(Integer, default=0)
    finish_video = Column(Integer, default=0)
    member = Column(Integer)
    date = Column(Date, default=datetime.date.today())
    style = Column(Integer)

    def __repr__(self):
        return '%d' % self.id

engine = create_engine('mysql+mysqldb://root:lsq65602597@localhost:3306/paint_master?charset=utf8')

DBSession = sessionmaker(bind=engine)


@celery.task
def neural(imageName, style):
    type = style
    song = '1'
    print type
    if (type == '1'):
        print type
        cmd01 = 'th neural_sand_server.lua -content_image input/' + imageName + '.jpg -output_image output/' + imageName + '.jpg'
        cmd02 = './video_sand output/' + imageName + '.jpg ' + imageName
        cmd03 = 'ffmpeg -i ' + imageName + '.avi -f mpeg ' + imageName + '.mp4'
        cmd04 = 'ffmpeg -i concat:"result1.mp4|'+ imageName +'.mp4" -c copy -f mpeg ' + imageName+ 'res.mp4'
        cmd05 = 'ffmpeg -i audio/' + song + '.mp3 -i video/' + imageName + 'res.mp4 -vcodec libx264 videores/' + imageName + '.mp4'
        res01 = subprocess.Popen(cmd01, shell=True, cwd=r'./static/a/')
        res01.wait()
        res02 = subprocess.Popen(cmd02, shell=True, cwd=r'./static/a/')
        res02.wait()
        res03 = subprocess.Popen(cmd03, shell=True, cwd=r'./static/a/video/')
        res03.wait()
        res04 = subprocess.Popen(cmd04, shell=True, cwd=r'./static/a/video')
        res04.wait()
        res05 = subprocess.Popen(cmd05, shell=True, cwd=r'./static/a/')
        res05.wait()
    elif(type == '2'):
        print type
        cmd = 'th neural_server.lua -content_image input/' + imageName + '.jpg -output_image output/' + imageName + '.jpg'
        res = subprocess.Popen(cmd, shell=True, cwd=r'./static/a/')
        res.wait()
    elif(type == '4'):
        print type
        cmd01 = './sketch input/' + imageName + '.jpg sketchout ' + imageName
        cmd02 = './sumiao ' + imageName
        res01 = subprocess.Popen(cmd01, shell=True, cwd=r'./static/a/')
        res01.wait()
        res02 = subprocess.Popen(cmd02, shell=True, cwd=r'./static/a/')
        res02.wait()
    session = DBSession()
    alter_img = session.query(Images).filter(Images.src_img == (imageName)).first()
    alter_img.finish_img = 1
    alter_img.finish_video = 1
    session.add(alter_img)
    session.commit()
    session.close()