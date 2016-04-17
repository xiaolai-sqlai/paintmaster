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
    userphone = request.cookies.get('userphone')
    if(userphone == None):
        return redirect(url_for('login'))
    return render_template('index.html')


@app.route('/login', methods=['GET', 'POST'])
def login():
    if(request.method == 'GET'):
        return render_template('login.html')
    else:
        userphone = request.form.get('userphone')
        password = request.form.get('password')
        session = DBSession()
        searchPassword = session.query(Member).filter(Member.userphone == userphone).first().password
        id = session.query(Member).filter(Member.userphone == userphone).first().id
        session.close()
        if(searchPassword == None):
            return '-1'
        elif(searchPassword != password):
            return '0'
        else:
            return str(id)


@app.route('/register', methods=['GET', 'POST'])
def register():
    if(request.method == 'GET'):
        return render_template('register.html')
    else:
        userphone = request.form.get('userphone')
        password = request.form.get('password')
        session = DBSession()
        new_member = Member(userphone=userphone, password=password)
        session.add(new_member)
        session.commit()
        id = session.query(Member).filter(Member.userphone == userphone).first().id
        session.close()
        resp = make_response( render_template('index.html') )
        resp.set_cookie('userphone', userphone)
        resp.set_cookie('id', str(id))
        return resp


@app.route('/gallery', methods=['GET', 'POST'])
def gallery():
    member = request.cookies.get('id')
    session = DBSession()
    images = session.query(Images).filter(Images.finish_img == 1, Images.member == member).all()
    videos = session.query(Images).filter(Images.finish_video == 1, Images.member == member).all()
    session.close()
    return render_template('gallery.html', images=images, videos=videos)


@app.route('/detail/<style>', methods=['GET'])
def detail(style):
    return render_template('detail.html', style=style)


@app.route('/upload/<style>', methods=['GET', 'POST'])
def upload(style):
    member = request.cookies.get('id')
    image = request.files['file']
    imageName = str(int(time.time())) + str(random.randint(10000, 99999))
    path = os.path.join(os.path.dirname(__file__), 'static', 'a', 'input', imageName + '.jpg')
    result = tasks.neural.apply_async(args=[imageName, style])
    image.save(path)
    session = DBSession()
    new_image = Images(src_img = imageName, member = member, style = style)
    session.add(new_image)
    session.commit()
    session.close()
    return redirect(url_for('index'))


@app.route('/image/<id>', methods=['GET', 'POST'])
def image(id):
    session = DBSession()
    img = session.query(Images).filter(Images.id == id).first()
    session.close()
    return render_template('image.html', show=img)

@app.route('/video/<id>', methods=['GET', 'POST'])
def video(id):
    session = DBSession()
    video = session.query(Images).filter(Images.id == id).first()
    session.close()
    return render_template('video.html', show=video)


if __name__ == '__main__':
    app.run(debug=True)
