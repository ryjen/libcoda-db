project "db"
    --uses "sqlite3"
    
    files {
        "db.h",
        "db.cpp"
    }
    excludes {
        "db.test.cpp"
    }