@echo off
cd /d %~dp0

if exist result rd /s /q result
md result

if exist Python34\python.exe (
	Python34\python.exe parse_xml_all.py
) else (
	python parse_xml_all.py
)

ping 127.0.0.1 -n 3 > nul
