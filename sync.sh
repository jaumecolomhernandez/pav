#!/usr/bin/env bash

rclone sync ~/pav/ drive_pav: --exclude ".git/**" -P

