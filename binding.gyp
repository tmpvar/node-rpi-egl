{
  "targets": [
    {
      "target_name": "binding",
      "sources": [ "src/binding.cc" ],
      "include_dirs": [
        "/opt/vc/include",
        "/opt/vc/include/interface/vcos/pthreads"
      ],
      "link_settings": {
        "libraries": [
         '-L/opt/vc/lib', '-lGLESv2', '-lEGL', '-lbcm_host'
        ]
      }
    }
  ]
}
