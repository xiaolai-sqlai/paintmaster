# encoding: utf-8
from flask import Flask, render_template, url_for, request, make_response, redirect, json
from sqlalchemy import create_engine, Column
from sqlalchemy.types import String, Integer, DateTime, Date
from sqlalchemy.orm import sessionmaker
from sqlalchemy.ext.declarative import declarative_base
import tasks
import sys, os, datetime, time, random, string, hashlib, urllib

reload(sys)
sys.setdefaultencoding("utf-8")

app = Flask(__name__)


Base = declarative_base()


class Member(Base):
    __tablename__ = 'members'
    id = Column(Integer, primary_key=True)
    userphone = Column(String(20))
    password = Column(String(40))
    vip = Column(Integer, default=0)
    piccount = Column(Integer, default=1)

    def __repr__(self):
        return '%d' % self.id


class Signature(Base):
    __tablename__ = 'wx_signature'
    id = Column(Integer, primary_key=True)
    access_token = Column(String(255))
    expire_time = Column(DateTime, default=(datetime.datetime.now() + datetime.timedelta(hours=2)).strftime('%y-%m-%d %I:%M:%S'))
    jsapi_ticket = Column(String(255))

    def __repr__(self):
        return '%d' % self.id


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


class Songs(Base):
    __tablename__ = 'songs'
    id = Column(Integer, primary_key=True)
    name = Column(String(100))

    def __repr__(self):
        return '%d' % self.id


class Tests(Base):
    __tablename__ = 'tests'
    id = Column(Integer, primary_key=True)
    name = Column(String(255))
    finish = Column(Integer, default=0)

    def __repr__(self):
        return '%d' % self.id


def getToken():
    appid = 'wx87064f1ba3830580'
    secret = '34e06abdd6ade4ed752046f34193326d'
    url = 'https://api.weixin.qq.com/cgi-bin/token?grant_type=client_credential&appid=' + appid + '&secret='+ secret
    access_token= json.loads(urllib.urlopen(url).read())['access_token']
    return access_token


def getTicket(access_token):
    jsurl = 'https://api.weixin.qq.com/cgi-bin/ticket/getticket?type=jsapi&access_token=' + access_token
    jsapi_ticket= json.loads(urllib.urlopen(jsurl).read())['ticket']
    return jsapi_ticket


# 初始化数据库连接:
engine = create_engine('mysql+mysqldb://root:lsq65602597@localhost:3306/paint_master?charset=utf8')
# 创建DBSession类型:
DBSession = sessionmaker(bind=engine)



@app.route('/', methods=['GET'])
def index():
    if(request.cookies.get('cookie')):
        return render_template('detail.html')
    resp = make_response(render_template('detail.html'))
    cookie = str(int(time.time())) + str(random.randint(10000, 99999))
    resp.set_cookie('cookie', cookie)
    return resp


@app.route('/wechat', methods=['GET', 'POST'])
def wehcat():
    return render_template('wechat.html')


@app.route('/wait', methods=['GET', 'POST'])
def wait():
    return render_template('wait.html')


@app.route('/upload', methods=['GET', 'POST'])
def upload():
    cookie = request.cookies.get('cookie')
    image = request.files['file']
    path = os.path.join(os.path.dirname(__file__), 'static', 'a', 'input', cookie + '.jpg')
    image.save(path)
    session = DBSession()
    if(session.query(Tests).filter(Tests.name == cookie).first().id):
        alter = session.query(Tests).filter(Tests.name==cookie).first()
        alter.finish = 0
        session.add(alter)
        session.commit()
    else:
        test = Tests(name = cookie)
        session.add(test)
        session.commit()
    session.close()
    result = tasks.neural.apply_async(args=[cookie])
    return redirect(url_for('wait'))


@app.route('/image', methods=['GET', 'POST'])
def image():
    cookie = request.cookies.get('cookie')
    session = DBSession()
    img = session.query(Tests).filter(Tests.name == cookie).order_by(Tests.id.desc()).first()
    session.close()
    return render_template('image.html', show=img)


if __name__ == '__main__':
    app.run(debug=True)
