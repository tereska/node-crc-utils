{
    "targets": [
        {
            "target_name": "crc32",
            "sources": [ "src/crc32.cc" ],
            "include_dirs": [
                "<!(node -e \"require('nan')\")"
            ],
        },
        {
            "target_name": "after_build",
            "type": "none",
            "dependencies": [
                "crc32"
            ],
            "copies": [
                {
                    "destination": "<(module_root_dir)/bin/",
                    "files": [
                        "<@(PRODUCT_DIR)/crc32.node"
                    ]
                }
            ]
        }
    ]
}

