```
FtpMessage {
    status: int
    statusMessage: string
    command : string
    userId : string
    parameters : dict
}
```


API:

    - Login(userId)
    - ListFiles(userId)
    - UploadFile()
    - DownloadFile(filename)