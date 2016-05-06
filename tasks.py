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

class Tests(Base):
    __tablename__ = 'tests'
    id = Column(Integer, primary_key=True)
    name = Column(String(255))
    finish = Column(Integer, default=0)

    def __repr__(self):
        return '%d' % self.id

engine = create_engine('mysql+mysqldb://root:lsq65602597@localhost:3306/paint_master?charset=utf8')

DBSession = sessionmaker(bind=engine)


@celery.task
def neural(imageName):
    session = DBSession()
    cmd01 = './water ' + imageName
    cmd02 = 'ffmpeg -i video/' + imageName + '.avi -y -f image2 -ss 0 -vframes 1 output/' +  imageName + '.jpg'
    cmd03 = 'ffmpeg -ss 0 -t 2 -i video/' + imageName + '.avi -s 480x640 -y -f gif output/' + imageName + '.gif'
    res01 = subprocess.Popen(cmd01, shell=True, cwd=r'./static/a/')
    res01.wait()
    res02 = subprocess.Popen(cmd02, shell=True, cwd=r'./static/a/')
    res02.wait()
    res03 = subprocess.Popen(cmd03, shell=True, cwd=r'./static/a/')
    res03.wait()
    alter_img = session.query(Tests).filter(Tests.name == imageName).first()
    alter_img.finish = 1
    session.add(alter_img)
    session.commit()
    session.close()