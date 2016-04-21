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
    session = DBSession()
    type = style
    song = '1'
    print type
    if(type == '1'):
        print type
        cmd01 = './sketch input/' + imageName + '.jpg sketchout ' + imageName
        cmd02 = './sumiao ' + imageName
        cmd03 = 'ffmpeg -i sumiaoout/image%d.jpg -r 30 videores/' + imageName + '.mp4'
        res01 = subprocess.Popen(cmd01, shell=True, cwd=r'./static/a/')
        res01.wait()
        res02 = subprocess.Popen(cmd02, shell=True, cwd=r'./static/a/')
        res02.wait()
        res03 = subprocess.Popen(cmd02, shell=True, cwd=r'./static/a/')
        res03.wait()
        alter_img = session.query(Images).filter(Images.src_img == (imageName)).first()
        alter_img.finish_img = 1
        session.add(alter_img)
        session.commit()
    elif(type == '2'):
        print type
        cmd01 = './water ' + imageName
        cmd02 = 'ffmpeg -i video/' + imageName + '.avi -y -f image2 -ss 0 -vframes 1 output/' +  imageName + '.jpg'
        cmd03 = 'ffmpeg -ss 0 -t 2 -i video/' + imageName + '.avi -s 480x640 -f gif output/' + imageName + '.gif'
        res01 = subprocess.Popen(cmd01, shell=True, cwd=r'./static/a/')
        res01.wait()
        res02 = subprocess.Popen(cmd02, shell=True, cwd=r'./static/a/')
        res02.wait()
        res03 = subprocess.Popen(cmd03, shell=True, cwd=r'./static/a/')
        res03.wait()
        alter_img = session.query(Images).filter(Images.src_img == (imageName)).first()
        alter_img.finish_img = 1
        session.add(alter_img)
        session.commit()
    elif (type == '3'):
        print type
        cmd01 = 'th neural_sand_server.lua -content_image input/' + imageName + '.jpg -output_image output/' + imageName + '.jpg'
        cmd02 = './video_sand output/' + imageName + '.jpg ' + imageName
        cmd03 = 'ffmpeg -i ' + imageName + '.avi -f mpeg ' + imageName + '.mp4'
        cmd04 = './sign ' + 'test'
        cmd05 = 'ffmpeg -i sign.avi -y -f mpeg video/sign.mp4'
        cmd06 = 'ffmpeg -i concat:"result1.mp4|' + imageName + '.mp4|sign.mp4" -c copy -f mpeg ' + imageName+ 'res.mp4'
        cmd07 = 'ffmpeg -i audio/' + song + '.mp3 -i video/' + imageName + 'res.mp4 -vcodec libx264 videores/' + imageName + '.mp4'
        res01 = subprocess.Popen(cmd01, shell=True, cwd=r'./static/a/')
        res01.wait()
        res02 = subprocess.Popen(cmd02, shell=True, cwd=r'./static/a/')
        res02.wait()
        res03 = subprocess.Popen(cmd03, shell=True, cwd=r'./static/a/video/')
        res03.wait()
        res04 = subprocess.Popen(cmd04, shell=True, cwd=r'./static/a/')
        res04.wait()
        res05 = subprocess.Popen(cmd05, shell=True, cwd=r'./static/a/')
        res05.wait()
        res06 = subprocess.Popen(cmd06, shell=True, cwd=r'./static/a/video/')
        res06.wait()
        res07 = subprocess.Popen(cmd07, shell=True, cwd=r'./static/a/')
        res07.wait()
        alter_img = session.query(Images).filter(Images.src_img == (imageName)).first()
        alter_img.finish_img = 1
        alter_img.finish_video = 1
        session.add(alter_img)
        session.commit()
    elif(type == '4'):
        print type
        cmd = 'th neural_server.lua -content_image input/' + imageName + '.jpg -output_image output/' + imageName + '.jpg'
        res = subprocess.Popen(cmd, shell=True, cwd=r'./static/a/')
        res.wait()
        alter_img = session.query(Images).filter(Images.src_img == (imageName)).first()
        alter_img.finish_img = 1
        session.add(alter_img)
        session.commit()
    session.close()