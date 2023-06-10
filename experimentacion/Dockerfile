FROM python:3.8.5

RUN useradd -m user

WORKDIR /home/user

USER user

RUN python3 -m pip install --upgrade pip
RUN pip3 install notebook==6.5.2
COPY requirements.txt requirements.txt
RUN pip3 install -r requirements.txt

EXPOSE 8888

WORKDIR /home/user/notebooks
ENTRYPOINT ["python3", "-m", "notebook", "-y", "--no-browser", "--ip", "0.0.0.0", "--port", "8888"]

