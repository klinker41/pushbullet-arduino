/*
 * Copyright (C) 2015 Jacob Klinker
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License
 */

package com.klinker.web.pushbullet

import grails.converters.JSON
import groovyx.net.http.ContentType
import groovyx.net.http.RESTClient

/**
 * Forwards information on users to Pushbullet through https://api.pushbullet.com/v2/users
 */
class UsersController {

	/**
	 * Find a JSON of the current user object based off of authorization header access token. More information can be
	 * found at https://docs.pushbullet.com/#users
	 *
	 * @return JSON containing current user
	 */
	def me() {
		def client = new RESTClient(PushbulletApi.BASE_URL)
		def resp

		if (request.get) {
			resp = client.get(
					path: 'v2/users/me',
					headers: ['Authorization': request.getHeader('Authorization')],
					requestContentType: ContentType.JSON
			)
		} else if (request.post) {
			// TODO should update user preferences
//			resp = client.post(
//					path: 'v2/users/me',
//					headers: ['Authorization': request.getHeader('Authorization')],
//					requestContentType: ContentType.JSON,
//					body: [
//					        key: 'value'
//					]
//			)
		}

		assert resp.status == 200
		render resp.getData() as JSON
		return resp.getData()
	}

}
